import os
import xml.etree.ElementTree as ET
import re

def sanitize_name(name):
    # Remove invalid characters and replace them with underscores
    return re.sub(r'[^A-Za-z0-9_]+', '_', name).strip('_')

def generate_enum_library(scripts_folder):
    root_folder = os.path.dirname(scripts_folder)  # Get the root folder of the project
    messages_folder = os.path.join(root_folder, 'Messages')  # Input folder for XML files
    output_folder = os.path.join(root_folder, 'Enum Libraries')  # Output folder for the enum libraries

    # Create output folder if it doesn't exist
    os.makedirs(output_folder, exist_ok=True)

    protocol_enums = {}

    # Traverse the directory to find XML files
    for file_name in os.listdir(messages_folder):
        if file_name.endswith('.xml'):
            file_path = os.path.join(messages_folder, file_name)
            tree = ET.parse(file_path)
            root = tree.getroot()

            # Iterate through each Message
            for message in root.findall('Message'):
                protocol = message.get('Protocol')
                key = message.get('Key')
                name = message.get('Name')
                description = message.get('Description', '')  # Get description if available

                # Create a valid enum name as Enum_Key
                enum_name = f"Enum_{key}"  # Name format: Enum_Key
                int_constant_name = sanitize_name(name).upper()  # Create constant name from the name
                
                # Create an entry for this protocol if not already created
                if protocol not in protocol_enums:
                    protocol_enums[protocol] = []

                enum_members = []

                # Check for Fields in the Message
                for field in message.findall('Field'):
                    member_name = sanitize_name(field.get('Name'))
                    logical_id = field.get('LogicalId')
                    enum_members.append(f"    {member_name} = {logical_id},")

                # Determine whether to create an enum or just an int constant
                if enum_members:
                    # Create the enum definition if there are fields
                    enum_description = (
                        f"/// Message Key {key}\n"
                        f"/// Name {name}\n"
                        f"/// Description {description}\n"
                        f"public enum {enum_name}\n{{\n" + "\n".join(enum_members) + "\n}}\n"
                    )

                    # Prepare the integer constant declaration.
                    int_constant = f"public const int Message_{key} = {key};\n"

                    # Append the enum followed by the int constant
                    protocol_enums[protocol].append(enum_description + int_constant)
                else:
                    # Only create the integer constant if there are no fields.
                    int_constant = f"/// Message Key {key}\n" \
                                   f"/// Name {name}\n" \
                                   f"/// Description {description}\n" \
                                   f"public const int Message_{key} = {key};\n"
                    
                    protocol_enums[protocol].append(int_constant)  # Append only the integer constant

    # Output the generated enumerations and constants to respective files
    for protocol, items in protocol_enums.items():
        library_name = os.path.join(output_folder, f"{protocol}EnumLibrary.cs")
        class_name = f"{protocol}EnumLibraryClass"  # Create class name based on protocol
        with open(library_name, 'w') as f:
            # Write the namespace declaration at the top
            f.write("namespace Generic.Enum_Libraries\n{\n")
            f.write(f"    public static class {class_name}\n    {{\n")  # Use the formatted class name

            # Write all items together
            for item in items:
                f.write(f"        {item}\n")  # Write each item
            
            f.write("    }\n")  # Close the class
            f.write("}\n")  # Close the namespace

if __name__ == "__main__":
    # Get the path of the directory where the script is located
    scripts_folder_path = os.path.dirname(os.path.realpath(__file__))
    generate_enum_library(scripts_folder_path)