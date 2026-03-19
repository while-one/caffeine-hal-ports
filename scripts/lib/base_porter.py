import argparse
import sys
from abc import ABC, abstractmethod
from pathlib import Path
from typing import Dict, Any

class BasePorter(ABC):
    """
    Abstract base class for hardware porters.
    Handles common pre-flight checks and provides an interface for vendor-specific scaffolding.
    """
    def __init__(self, args: Dict[str, Any]):
        self.args = args
        self.vendor = args.get('vendor')
        self.family = args.get('family')
        self.project_root = Path.cwd()

    def validate_environment(self):
        """Common pre-flight checks for all vendors."""
        if not (self.project_root / "CMakeLists.txt").exists():
            print("Error: This script must be run from the root of caffeine-hal-ports.")
            sys.exit(1)
        
        if not (self.project_root / "src").exists():
            print("Error: Could not find 'src' directory.")
            sys.exit(1)

    @abstractmethod
    def perform_checks(self):
        """Vendor-specific pre-flight checks."""
        pass

    @abstractmethod
    def scaffold(self):
        """Vendor-specific scaffolding implementation."""
        pass

    def run(self):
        """Execute the porting workflow."""
        self.validate_environment()
        self.perform_checks()
        self.scaffold()
