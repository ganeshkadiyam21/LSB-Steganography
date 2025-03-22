**Project Description**
This project implements Least Significant Bit (LSB) Steganography in C to securely embed and extract secret messages within digital images. The technique ensures minimal distortion, making it an effective method for covert communication and secure data transmission.
**Motivation**
 With the increasing need for secure communication, steganography provides an additional layer of security by hiding messages within images. Unlike encryption, which can attract attention, LSB steganography conceals the existence of the message itself.
 **Features**
Embed Secret Messages: Hides text within an image using LSB substitution.
Extract Hidden Data: Retrieves the embedded message without noticeable image changes.
Minimal Distortion: Maintains image quality while securely storing data.
Optimized Encoding & Decoding: Efficiently processes images to hide and reveal messages.
Customizable Message Length: Can adjust message capacity based on image size.
**Tech Stack**
Programming Language: C.
Concepts Used: Bitwise Operations, File Handling, Data Hiding Techniques.
**How It Works**
Encoding Process:
Convert the secret message into binary format.
Open the BMP image and extract pixel data.
Modify the least significant bit of each pixel to encode the message bits.
Save the modified image as the output file.
Decoding Process:
Load the stego image and extract pixel data.
Retrieve the least significant bits from each pixel.
Convert the retrieved bits back into readable text.
**Use Cases**
Covert Communication: Secretly exchange messages without suspicion.
Digital Watermarking: Embed copyright or authentication information in images.
Forensics & Cybersecurity: Hide sensitive information within media files.
Steganographic Cryptography: Combine encryption and steganography for layered security.

