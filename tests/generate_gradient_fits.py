import numpy as np

def save_fits(filename, data):
    h, w = data.shape
    header = []
    def add_card(key, val=None, comment=""):
        if val is None:
            card = f"{key:<8}"
        elif isinstance(val, bool):
            val_str = 'T' if val else 'F'
            card = f"{key:<8}= {val_str:>20} / {comment}"
        elif isinstance(val, int):
            card = f"{key:<8}= {val:>20} / {comment}"
        elif isinstance(val, str):
            card = f"{key:<8}= '{val:<8}' / {comment}"
        else:
            card = f"{key:<8}"
        card = f"{card:<80}"
        header.append(card.encode('ascii'))
    
    add_card("SIMPLE", True, "conforms to FITS standard")
    add_card("BITPIX", -32, "array data type")
    add_card("NAXIS", 2, "number of array dimensions")
    add_card("NAXIS1", w, "length of dimension 1")
    add_card("NAXIS2", h, "length of dimension 2")
    add_card("EXTEND", True, "FITS dataset may contain extensions")
    add_card("END")
    
    header_bytes = b"".join(header)
    pad_len = (2880 - (len(header_bytes) % 2880)) % 2880
    header_bytes += b" " * pad_len
    
    data_bytes = data.astype('>f4').tobytes()
    pad_len = (2880 - (len(data_bytes) % 2880)) % 2880
    data_bytes += b"\x00" * pad_len
    
    with open(filename, 'wb') as f:
        f.write(header_bytes)
        f.write(data_bytes)

# Create a 512x512 image
h, w = 512, 512
y, x = np.mgrid[0:h, 0:w]

# Base gradient: plane stretching from 0.1 to 0.6
gradient = 0.1 + 0.3 * (x / (w - 1)) + 0.2 * (y / (h - 1))

# Add some high frequency noise
noise = np.random.normal(0, 0.01, (h, w))

# Add a couple of synthetic stars (Gaussian)
star1 = 1.0 * np.exp(-((x - 150)**2 + (y - 200)**2) / (2 * 5**2))
star2 = 1.5 * np.exp(-((x - 350)**2 + (y - 300)**2) / (2 * 7**2))

data = gradient + noise + star1 + star2

save_fits("gradient_input.fits", data)
print("Saved gradient_input.fits successfully.")
