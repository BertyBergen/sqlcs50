import unittest
from run_script import run_script, clean_output

class TestDuplicateID(unittest.TestCase):
    def test_prints_error_message_if_duplicate_id(self):
        script = [
            "insert 1 user1 person1@example.com;",
            "insert 1 user1 person1@example.com;",
            "select;",
            ".exit;",
        ]
        result = run_script(script)
        cleaned = clean_output(result)
        self.assertIn("Error: Duplicate key.", cleaned)
