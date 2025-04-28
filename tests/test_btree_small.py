import unittest
from run_script_helper import run_script, clean_output

class TestBtreeSmall(unittest.TestCase):
    def test_prints_structure_of_one_node_btree(self):
        script = [
            "insert 3 user3 person3@example.com;",
            "insert 1 user1 person1@example.com;",
            "insert 2 user2 person2@example.com;",
            ".btree;",
            ".exit;",
        ]
        result = run_script(script)
        cleaned = clean_output(result)
        self.assertIn("Tree:", cleaned)
        self.assertIn("- leaf (size 3)", cleaned)

    def test_prints_structure_of_three_leaf_node_btree(self):
        script = [f"insert {i} user{i} person{i}@example.com;" for i in range(1, 15)]
        script += [
            ".btree;",
            "insert 15 user15 person15@example.com;",
            ".exit;",
        ]
        result = run_script(script)
        cleaned = clean_output(result)
        self.assertIn("- internal (size 1)", cleaned)
