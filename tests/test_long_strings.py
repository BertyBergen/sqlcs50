import unittest
import os
from run_script import run_script, clean_output


test_name = os.path.basename(__file__)
class TestLongStrings(unittest.TestCase):
    def test_inserting_max_length_strings(self):
        long_username = "a" * 32
        long_email = "a" * 255
        script = [
            f"insert 1 {long_username} {long_email};",
            "select;",
            ".exit",
        ]
        result = run_script(script,test_name, "test_inserting_max_length_strings")        
        self.assertEqual(["sqlcs50> Executed.",f"sqlcs50> (1, {long_username}, {long_email})", "Executed.", "sqlcs50>"], result)

    def test_error_message_if_strings_are_too_long(self):
        long_username = "a" * 33
        long_email = "a" * 256
        script = [
            f"insert 1 {long_username} {long_email};",
            "select;",
            ".exit;",
        ]
        result = run_script(script,test_name, "test_error_message_if_strings_are_too_long")
        self.assertIn("sqlcs50> String is too long.", result)

