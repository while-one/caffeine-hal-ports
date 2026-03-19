import os
import sys
import re
from pathlib import Path
from typing import Dict, Any
from .base_porter import BasePorter

class Stm32Porter(BasePorter):
    """
    STM32 specific implementation of the porting logic.
    """
    
    def __init__(self, args: Dict[str, Any]):
        super().__init__(args)
        self.family_short = self.family.replace("stm32", "")
        self.family_short_upper = self.family_short.upper()
        self.family_name_upper = self.family.upper()
        self.part_number = args['part_number']
        self.template_dir = Path(__file__).parent / "templates" / "stm32"

    def perform_checks(self):
        """STM32 specific pre-flight checks."""
        family_dir = self.project_root / "src" / "stm32" / self.family
        if family_dir.exists():
            print(f"Error: Directory {family_dir} already exists. Port might already be present.")
            sys.exit(1)

        recipe_file = self.project_root / "cmake" / "ports" / "stm32" / f"{self.family}.cmake"
        if recipe_file.exists():
            print(f"Error: Recipe file {recipe_file} already exists.")
            sys.exit(1)

        print(f"Pre-flight checks passed for STM32 family: {self.family}")

    def load_script_template(self, name: str) -> str:
        """Load a script-specific template."""
        template_path = self.template_dir / f"{name}.template"
        try:
            with open(template_path, "r") as f:
                return f.read()
        except FileNotFoundError:
            print(f"Error: Template file {template_path} not found.")
            sys.exit(1)

    def transform_c_template(self, content: str, peripheral: str) -> str:
        """Transform a generic C template into STM32 specific implementation."""
        periph_upper = peripheral.upper()
        module_define = f"HAL_{periph_upper}_MODULE_ENABLED"
        hal_header = f'#include "stm32{self.family_short}xx_hal.h"'
        
        # 1. Insert STM32 HAL header
        content = content.replace(f'#include "cfn_hal_{peripheral}_port.h"', 
                                  f'#include "cfn_hal_{peripheral}_port.h"\n{hal_header}')

        # 2. Wrap implementation in guards
        construct_pattern = rf"cfn_hal_{peripheral}_construct\(.*?\)\s*{{.*?}}"
        destruct_pattern = rf"cfn_hal_{peripheral}_destruct\(.*?\)\s*{{.*?}}"
        
        construct_match = re.search(construct_pattern, content, re.DOTALL)
        destruct_match = re.search(destruct_pattern, content, re.DOTALL)
        
        if not construct_match or not destruct_match:
            print(f"Warning: Could not find construct/destruct in cfn_hal_{peripheral}_port.c template.")
            return content

        construct_body = construct_match.group(0)
        destruct_body = destruct_match.group(0)
        
        impl_part = content.replace(construct_body, "").replace(destruct_body, "")
        headers_end = impl_part.find(hal_header) + len(hal_header)
        
        final_content = impl_part[:headers_end] + f"\n\n#ifdef {module_define}\n" + impl_part[headers_end:]
        final_content += f"\n{construct_body}\n\n{destruct_body}\n"
        
        final_content += f"""
#else

cfn_hal_error_code_t cfn_hal_{peripheral}_construct(cfn_hal_{peripheral}_t *driver, 
                                              const cfn_hal_{peripheral}_config_t *config, 
                                              const cfn_hal_{peripheral}_phy_t *phy)
{{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(config);
    CFN_HAL_UNUSED(phy);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}}

cfn_hal_error_code_t cfn_hal_{peripheral}_destruct(cfn_hal_{peripheral}_t *driver)
{{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}}

#endif /* {module_define} */
"""
        return final_content

    def scaffold(self):
        """Perform the STM32 scaffolding."""
        family_path = self.project_root / "src" / "stm32" / self.family
        family_path.mkdir(parents=True, exist_ok=True)
        (self.project_root / "cmake" / "ports" / "stm32").mkdir(parents=True, exist_ok=True)
        (self.project_root / "cmake" / "presets" / "stm32").mkdir(parents=True, exist_ok=True)

        print(f"Creating scaffolding for {self.family}...")

        # 1. HAL Conf Template
        conf_template = self.load_script_template("hal_conf.h.in")
        with open(family_path / f"stm32{self.family_short}xx_hal_conf.h.in", "w") as f:
            f.write(conf_template.format(
                family_short=self.family_short,
                family_short_upper=self.family_short_upper,
                family_name=self.family
            ))

        # 2. Port Recipe
        port_template = self.load_script_template("port.cmake")
        with open(self.project_root / "cmake" / "ports" / "stm32" / f"{self.family}.cmake", "w") as f:
            f.write(port_template.format(
                family_name=self.family,
                family_name_upper=self.family_name_upper,
                family_short=self.family_short,
                family_short_upper=self.family_short_upper,
                sdk_url=self.args['sdk_url'],
                sdk_tag=self.args['sdk_tag'],
                mcu_macro=self.args['mcu_macro']
            ))

        # 3. Preset JSON
        preset_template = self.load_script_template("presets.json")
        with open(self.project_root / "cmake" / "presets" / "stm32" / f"{self.family}.json", "w") as f:
            f.write(preset_template.format(
                family_name=self.family,
                family_name_upper=self.family_name_upper,
                part_number=self.part_number,
                part_number_lower=self.part_number.lower(),
                core=self.args['core'],
                mcu_macro=self.args['mcu_macro'],
                compile_options=self.args.get('compile_options', '')
            ))

        # 4. Transform and Copy templates
        template_src_dir = self.project_root / "src" / "template"
        peripherals = ['adc', 'gpio', 'uart']
        
        for periph in peripherals:
            c_template = template_src_dir / f"cfn_hal_{periph}_port.c"
            h_template = template_src_dir / f"cfn_hal_{periph}_port.h"
            
            if c_template.exists():
                with open(c_template, "r") as f:
                    content = f.read()
                transformed = self.transform_c_template(content, periph)
                with open(family_path / f"cfn_hal_{periph}_port.c", "w") as f:
                    f.write(transformed)
            
            if h_template.exists():
                with open(h_template, "r") as f:
                    content = f.read()
                with open(family_path / f"cfn_hal_{periph}_port.h", "w") as f:
                    f.write(content)

        print("\nSTM32 Scaffolding complete!")
        print(f"Next steps:")
        print(f"1. Add 'cmake/presets/stm32/{self.family}.json' to the 'include' list in root CMakePresets.json.")
        print(f"2. Run: cmake --preset {self.part_number.lower()}-release")
        print(f"3. Implement VMT wrappers in {family_path}/")
