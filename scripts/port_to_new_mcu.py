#!/usr/bin/env python3
"""
Hardware Port Scaffolding Entry Point for Caffeine HAL Ports.

This script manages the high-level workflow for adding new MCU families.
It handles common argument parsing and dispatches to vendor-specific porters.

Usage:
    python3 port_to_new_mcu.py --vendor stm32 --family stm32h7 ...
"""

import argparse
import json
import sys
from typing import Dict, Any
from lib.stm32_porter import Stm32Porter

def main():
    parser = argparse.ArgumentParser(description="Scaffold a new MCU family port.")
    
    # Common Args
    parser.add_argument("--json", type=str, help="Path to a JSON configuration file.")
    parser.add_argument("--vendor", type=str, choices=['stm32'], help="MCU Vendor.")
    parser.add_argument("--family", type=str, help="Target family name (e.g., stm32h7).")
    
    # Vendor Specific (Optional here, validated by porter)
    parser.add_argument("--sdk_url", type=str, help="Vendor SDK GitHub URL.")
    parser.add_argument("--sdk_tag", type=str, help="Vendor SDK Git Tag.")
    parser.add_argument("--mcu_macro", type=str, help="Target MCU Macro (e.g., STM32H743xx).")
    parser.add_argument("--part_number", type=str, help="Full Part Number (e.g., STM32H743VIT6).")
    parser.add_argument("--core", type=str, help="Target CPU Core (e.g., cortex-m7).")
    parser.add_argument("--compile_options", type=str, default="", help="Optional compile options.")

    args_ns = parser.parse_args()
    
    if args_ns.json:
        with open(args_ns.json, 'r') as f:
            args = json.load(f)
    else:
        args = vars(args_ns)

    vendor = args.get('vendor')
    if not vendor:
        print("Error: --vendor is required.")
        sys.exit(1)

    # Dispatch to appropriate porter
    if vendor == 'stm32':
        porter = Stm32Porter(args)
    else:
        print(f"Error: Vendor '{vendor}' is not supported yet.")
        sys.exit(1)

    porter.run()

if __name__ == "__main__":
    main()
