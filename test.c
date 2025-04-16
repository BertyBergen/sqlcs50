+  it 'prints constants' do
+    script = [
+      ".constants",
+      ".exit",
+    ]
+    result = run_script(script)
+
+    expect(result).to match_array([
+      "db > Constants:",
+      "ROW_SIZE: 293",
+      "COMMON_NODE_HEADER_SIZE: 6",
+      "LEAF_NODE_HEADER_SIZE: 10",
+      "LEAF_NODE_CELL_SIZE: 297",
+      "LEAF_NODE_SPACE_FOR_CELLS: 4086",
+      "LEAF_NODE_MAX_CELLS: 13",
+      "db > ",
+    ])
+  end