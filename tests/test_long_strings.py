import unittest
from run_script_helper import run_script, clean_output

class TestLongStrings(unittest.TestCase):
    def test_allows_inserting_strings_that_are_max_length(self):
        long_username = "a" * 32
        long_email = "a" * 255
        script = [
            f"insert 1 {long_username} {long_email};",
            "select;",
            ".exit;",
        ]
        result = run_script(script)
        cleaned = clean_output(result)
        self.assertEqual(cleaned, [
            "Executed.",
            f"(1, {long_username}, {long_email})",
            "Executed.",
            "",
        ])

    def test_prints_error_message_if_strings_are_too_long(self):
        long_username = "a" * 33
        long_email = "a" * 256
        script = [
            f"insert 1 {long_username} {long_email};",
            "select;",
            ".exit;",
        ]
        result = run_script(script)
        self.assertIn("sqlcs50 > String is too long.", result)
