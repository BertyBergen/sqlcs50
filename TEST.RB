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

    +  it 'allows printing out the structure of a one-node btree' do
        +    script = [3, 1, 2].map do |i|
        +      "insert #{i} user#{i} person#{i}@example.com"
        +    end
        +    script << ".btree"
        +    script << ".exit"
        +    result = run_script(script)
        +
        +    expect(result).to match_array([
        +      "db > Executed.",
        +      "db > Executed.",
        +      "db > Executed.",
        +      "db > Tree:",
        +      "leaf (size 3)",
        +      "  - 0 : 3",
        +      "  - 1 : 1",
        +      "  - 2 : 2",
        +      "db > "
        +    ])
        +  end