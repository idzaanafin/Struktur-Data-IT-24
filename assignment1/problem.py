from Crypto.Util.strxor import strxor
from Crypto.Cipher import AES
from pwn import xor
# S-Box (digunakan di SubBytes)
sbox = [
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
]


# Inverse S-Box (untuk dekripsi)
invsbox = [sbox.index(x) for x in range(256)]

# MixColumns matriks
mix_columns_matrix = [
    [2, 3, 1, 1],
    [1, 2, 3, 1],
    [1, 1, 2, 3],
    [3, 1, 1, 2]
]

# Inverse MixColumns matriks
inv_mix_columns_matrix = [
    [0x0e, 0x0b, 0x0d, 0x09],
    [0x09, 0x0e, 0x0b, 0x0d],
    [0x0d, 0x09, 0x0e, 0x0b],
    [0x0b, 0x0d, 0x09, 0x0e]
]

def sub_bytes(state, sbox_table):
    return [[sbox_table[byte] for byte in row] for row in state]

def reverse_rows(state):
 #   temp=state[3]
#    state[3] =state[1]
  #  state[1] = temp
    new= state[:4]+state[4:8][::-1]+state[8:12]+state[12:16][::-1]
    print(new)
    return new

def mix_columns(state, matrix):
    def gmul(a, b):
        p = 0
        for i in range(8):
            if b & 1:
                p ^= a
            hi_bit_set = a & 0x80
            a <<= 1
            if hi_bit_set:
                a ^= 0x1b
            b >>= 1
        return p % 256

    result = [[0] * 4 for _ in range(4)]
    for i in range(4):
        for j in range(4):
            result[i][j] = gmul(matrix[i][0], state[0][j]) ^ gmul(matrix[i][1], state[1][j]) ^ gmul(matrix[i][2], state[2][j]) ^ gmul(matrix[i][3], state[3][j])
    return result

def add_round_key(state, key):
    return [[state[i][j] ^ key[i][j] for j in range(4)] for i in range(4)]

def one_round_aes_encrypt(plaintext, round_key):
    state = [list(plaintext[i:i+4]) for i in range(0, 16, 4)]
    state = sub_bytes(state, sbox)
#    print(state)
    state = reverse_rows(state)
#    print(state)
    state = mix_columns(state, mix_columns_matrix)
    round_key_matrix = [list(round_key[i:i+4]) for i in range(0, 16, 4)]
  #  print(round_key_matrix)
    print(state)
    state = add_round_key(state, round_key_matrix)
    print(state)
    return bytes(sum(state, []))

def one_round_aes_decrypt(ciphertext, round_key):
    state = [list(ciphertext[i:i+4]) for i in range(0, 16, 4)]
    round_key_matrix = [list(round_key[i:i+4]) for i in range(0, 16, 4)]
    state = add_round_key(state, round_key_matrix)
    state = mix_columns(state, inv_mix_columns_matrix)
    state = reverse_rows(state)
    state = sub_bytes(state, invsbox)
    return bytes(sum(state, []))

# Contoh penggunaan
plaintext = b"Hello, World!aaa"
round_key = b"1234567890abcdef"
ciphertext = one_round_aes_encrypt(plaintext, round_key)
decrypted = one_round_aes_decrypt(ciphertext, round_key)

print("Plaintext:", plaintext)
print("Ciphertext:", ciphertext)
print("Decrypted:", decrypted)
