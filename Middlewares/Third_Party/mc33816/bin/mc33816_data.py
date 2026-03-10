#!/usr/bin/env python3
"""
Script to update MC33816_data.c arrays from hex files.
Reads the hex files mentioned in comments and updates the corresponding arrays.
"""

import re
import os

# Mapping of array names to their source hex files
ARRAY_FILE_MAPPING = {
    'MC33816_code_RAM1': ['ch1.cip.hex'],
    'MC33816_code_RAM2': ['ch2.cip.hex'],
    'MC33816_data_RAM': ['dram1.hex', 'dram2.hex'],  # Combined from two files
    'MC33816_main_config': ['main_config_reg.hex'],
    'MC33816_ch1_config': ['ch1_config_reg.hex'],
    'MC33816_ch2_config': ['ch2_config_reg.hex'],
    'MC33816_io_config': ['io_config_reg.hex'],
    'MC33816_diag_config': ['diag_config_reg.hex']
}

def parse_hex_file(filepath):
    """Parse a hex file and extract hex values."""
    values = []
    
    if not os.path.exists(filepath):
        print(f"Warning: File not found: {filepath}")
        return values
    
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Find all hex values in format 0xXXXX
    hex_pattern = r'0x[0-9A-Fa-f]{4}'
    matches = re.findall(hex_pattern, content)
    values.extend(matches)
    
    return values

def format_array_values(values, indent=4, values_per_line=10):
    """Format array values for C code."""
    if not values:
        return ""
    
    lines = []
    indent_str = ' ' * indent
    
    for i in range(0, len(values), values_per_line):
        line_values = values[i:i + values_per_line]
        # Add comma after each value except potentially the last one overall
        formatted_values = []
        for j, val in enumerate(line_values):
            global_idx = i + j
            if global_idx < len(values) - 1:
                formatted_values.append(f"{val}")
            else:
                formatted_values.append(f"{val}")
        
        line = indent_str + ', '.join(formatted_values)
        if i + values_per_line < len(values):
            line += ','
        lines.append(line)
    
    return '\n'.join(lines)

def read_c_file(filepath):
    """Read the C file content."""
    with open(filepath, 'r', encoding='utf-8') as f:
        return f.read()

def write_c_file(filepath, content):
    """Write content to the C file."""
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)

def update_array_in_content(content, array_name, new_values):
    """Update a specific array in the C file content."""
    if not new_values:
        print(f"Warning: No values to update for {array_name}")
        return content
    
    # Pattern to match array declaration with size and values
    # Format: unsigned short array_name[size] = \n{ values };
    pattern = rf'(unsigned short {re.escape(array_name)}\[\d+\]\s*=\s*\n\{{\n)(.*?)(\n\}};)'
    
    match = re.search(pattern, content, re.DOTALL)
    if not match:
        print(f"Warning: Could not find array {array_name} in file")
        return content
    
    # Format new values
    formatted_values = format_array_values(new_values)
    
    # Replace the array values
    new_content = content[:match.start(2)] + formatted_values + content[match.end(2):]
    
    print(f"✓ Updated {array_name} with {len(new_values)} values")
    return new_content

def main():
    """Main function to update all arrays."""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    c_file_path = os.path.join(script_dir, 'MC33816_data.c')
    
    print(f"Reading C file: {c_file_path}")
    content = read_c_file(c_file_path)
    original_content = content
    
    # Process each array
    for array_name, hex_files in ARRAY_FILE_MAPPING.items():
        print(f"\nProcessing {array_name}...")
        
        # Read all hex files for this array
        all_values = []
        for hex_file in hex_files:
            hex_path = os.path.join(script_dir, hex_file)
            print(f"  Reading {hex_file}...")
            values = parse_hex_file(hex_path)
            print(f"  Found {len(values)} values in {hex_file}")
            all_values.extend(values)
        
        # Update the array
        if all_values:
            content = update_array_in_content(content, array_name, all_values)
    
    # Write updated content
    if content != original_content:
        write_c_file(c_file_path, content)
        print(f"\n✓ Successfully updated {c_file_path}")
    else:
        print("\n⚠ No changes were made to the file")

if __name__ == '__main__':
    main()
