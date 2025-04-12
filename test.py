import unittest
import subprocess

class TestDatabase(unittest.TestCase):
    def run_script(self, commands):

        input_script = "\n".join(commands) + "\n"
        
        try:
            process = subprocess.Popen(
                "./qwe.exe",
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            raw_output, _ = process.communicate(input=input_script, timeout=10)
            return [line.strip() for line in raw_output.strip().split("\n")]
        except subprocess.TimeoutExpired:
            process.kill()
            self.fail("The program took too long to respond.")

        with subprocess.Popen(
            "./qwe.exe",
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        ) as process:
            for command in commands:
                process.stdin.write(command + "\n")
            process.stdin.flush()
            process.stdin.close()

            try:
                raw_output, _ = process.communicate(timeout=10)
            except subprocess.TimeoutExpired:
                process.kill()
                self.fail("The program took too long to respond.")
        
        return [line.strip() for line in raw_output.strip().split("\n")]


    def test_insert_and_select(self):
        result = self.run_script([
            "insert 1 user1 person1@example.com;",
            "select;",
            ".exit;",
        ])
        self.assertListEqual(result, [
            "sqlcs50 > Executed.",
            "sqlcs50 > (1, user1, person1@example.com)",
            "Executed.",
            "sqlcs50 >",
        ])

    def test_prints_error_message_when_table_is_full(self):
        script = [f"insert {i} user{i} person{i}@example.com;" for i in range(1, 1402)]
        script.append(".exit;")
        result = self.run_script(script)
        self.assertIn("sqlcs50 > Error: Table full.", result)

# INCORRECT

    def test_allows_inserting_strings_that_are_max_length(self):
        long_username = "a" * 32
        long_email = "a" * 255
        script = [
            f"insert 1 {long_username} {long_email};",
            "select;",
            ".exit;",
        ]
        result = self.run_script(script)
        self.assertListEqual(result, [
            "sqlcs50 > Executed.",
            f"sqlcs50 > (1, {long_username}, {long_email})",
            "Executed.",
            "sqlcs50 >",
        ])

    def test_prints_error_message_if_strings_are_too_long(self):
        long_username = "a" * 33
        long_email = "a" * 256
        script = [
            f"insert 1 {long_username} {long_email};",
            "select;",
            ".exit;",
        ]
        result = self.run_script(script)
        self.assertIn("sqlcs50 > String is too long.", result)

    def test_prints_error_message_if_id_is_negative(self):
        script = [
            "insert -1 cstack foo@bar.com;",
            "select;",
            ".exit;",
        ]
        result = self.run_script(script)
        self.assertIn("sqlcs50 > ID must be positive.", result)

if __name__ == "__main__":
    unittest.main()
