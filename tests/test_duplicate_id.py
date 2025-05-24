import unittest
import os
from run_script import run_script, clean_output

test_name = os.path.basename(__file__)

class TestDuplicateID(unittest.TestCase):
    def test_if_duplicate_id(self):
        script = [
            "insert 1 user1 person1@example.com;",
            "insert 1 user1 person1@example.com;",
            "select;",
            ".exit;",
        ]
        result = run_script(script, test_name, "test_if_duplicate_id")
        cleaned = clean_output(result)
        self.assertIn("Error: Duplicate key.", cleaned)
