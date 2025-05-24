import unittest
import os
from run_script import run_script

test_name = os.path.basename(__file__)
class TestNegativeID(unittest.TestCase):
    def test_if_id_is_negative(self):
        script = [
            "insert -1 sysna kachaka@bar.com;",
            "select;",
            ".exit;",
        ]
        result = run_script(script, test_name,  "test_if_id_is_negative")
        self.assertIn("sqlcs50> ID must be positive.", result)
