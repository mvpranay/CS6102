import numpy as np
from scipy.stats import multivariate_normal

team_number = 7  # Change team number to your team number

if team_number is None:
    raise NotImplementedError("Please set your team_number before running the script.")

sbox = [
    # 0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f 
    0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76, # 0
    0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0, # 1
    0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15, # 2
    0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75, # 3
    0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84, # 4
    0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf, # 5
    0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8, # 6
    0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2, # 7
    0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73, # 8
    0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb, # 9
    0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79, # a
    0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08, # b
    0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a, # c
    0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e, # d
    0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf, # e
    0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16  # f
]
sbox = np.array(sbox, dtype=np.uint8)


def align_traces(data: np.ndarray) -> np.ndarray:

    n_traces, n_samples = data.shape
    reference = data[0]  # shape: (n_samples,)

    # --- FFT-based cross-correlation (fully vectorized) ---
    # Pad to avoid circular correlation artifacts
    fft_len = 2 * n_samples - 1  # minimum; next power of 2 is faster
    fft_len = 1 << (fft_len - 1).bit_length()  # next power of 2

    # FFT of reference (conjugate) and all traces
    ref_fft  = np.fft.rfft(reference, n=fft_len)           # (fft_len//2+1,)
    data_fft = np.fft.rfft(data,      n=fft_len, axis=1)   # (n_traces, fft_len//2+1)

    # Cross-correlation in frequency domain: conj(REF) * TRACE
    corr_fft = np.conj(ref_fft)[np.newaxis, :] * data_fft  # broadcast over traces

    # Back to time domain
    corr = np.fft.irfft(corr_fft, n=fft_len, axis=1)       # (n_traces, fft_len)

    # Rearrange so lag=0 is at center (like 'full' mode output)
    corr = np.concatenate([corr[:, -(n_samples - 1):], corr[:, :n_samples]], axis=1)
    # corr shape: (n_traces, 2*n_samples - 1)

    # Lag at peak correlation for each trace
    lags = np.argmax(corr, axis=1) - (n_samples - 1)       # (n_traces,) signed lags

    # --- Apply shifts with zero-padding (vectorized) ---
    aligned = np.zeros_like(data)
    idx = np.arange(n_samples)  # column indices

    for i, lag in enumerate(lags):
        if lag == 0:
            aligned[i] = data[i]
        elif lag > 0:
            # Trace is late → shift left; zero-pad on the right
            aligned[i, :n_samples - lag] = data[i, lag:]
        else:
            # Trace is early → shift right; zero-pad on the left
            lag = -lag
            aligned[i, lag:] = data[i, :n_samples - lag]

    return aligned

data = np.load("profiling_traces.npz")

trace_array = np.trim_zeros(align_traces(data["traces"]))
textin_array = data["textin"]
textout_array = data["textout"]
key_array = data["keys"]

data = np.load(f"attack_traces_team_{team_number}.npz") 

attack_traces = np.trim_zeros(align_traces(data["traces"]))
attack_textins = data["textin"]
attack_textouts = data["textout"]

sbox_outputs = sbox[textin_array ^ key_array]
hamming_weights = np.bitwise_count(sbox_outputs)

HW = np.array([bin(x).count("1") for x in range(256)])

def compute_class_stats(X_pois:np.ndarray, hw, num_classes=9):
    d = X_pois.shape[1]
    means = np.zeros((num_classes, d))
    covs = np.zeros((num_classes, d, d))

    for k in range(num_classes):
        # (num_in_class, d)
        X_k = X_pois[hw == k, :]
        if X_k.shape[0] == 0:
            continue

        mu_k = np.mean(X_k, axis=0)
        cov_k = np.cov(X_k, rowvar=False, bias=False)
        cov_k += 1e-6 * np.eye(d)  # regularize to avoid singular matrices
        means[k] = mu_k
        covs[k] = cov_k

    return means, covs

# finding PoI

def compute_labels(plaintexts, key_byte, byte_idx):
    # plaintexts: (N, 16)
    p = plaintexts[:, byte_idx]
    return HW[sbox[p ^ key_byte]]

def compute_snr(traces, labels, num_classes=9):
    """
    traces: shape (N, T)
    labels: shape (N,) with values in [0, num_classes-1]
    """
    N, T = traces.shape
    
    # Mean per class
    means = np.zeros((num_classes, T))
    variances = np.zeros((num_classes, T))
    counts = np.zeros(num_classes)
    
    for c in range(num_classes):
        class_traces = traces[labels == c]
        
        if len(class_traces) == 0:
            continue
        
        means[c] = np.mean(class_traces, axis=0)
        variances[c] = np.var(class_traces, axis=0)
        counts[c] = len(class_traces)
    
    # Between-class variance
    mean_total = np.sum(means * counts[:, None], axis=0) / np.sum(counts)
    var_between = np.sum(counts[:, None] * (means - mean_total)**2, axis=0) / np.sum(counts)
    
    # Within-class variance
    var_within = np.sum(counts[:, None] * variances, axis=0) / np.sum(counts)
    # Avoid division by zero
    snr = var_between / (var_within + 1e-12)
    return snr

def get_pois(snr, k=10):
    return np.argsort(snr)[-k:]

answer = []

for byte in range(16):

    labels = compute_labels(textin_array, key_array[0][byte], byte)
    snr = compute_snr(trace_array, labels)
    pois = get_pois(snr,5)
    means, covs = compute_class_stats(trace_array[:,pois], hamming_weights[:, byte])

    total_likelihood = np.zeros(256)

    # loop over keys
    for kb in range(256):
        log_likelihood = 0
        # loop over attack traces
        for atk_trace, atk_textin in zip(attack_traces, attack_textins):
            # calculate attack trace's PoI values
            v = HW[sbox[atk_textin[byte]^kb]]
            x = atk_trace[pois] 
            # add the likelihood of key to total likelihood for that key
            log_likelihood += multivariate_normal.logpdf(x,means[v],covs[v]) # type: ignore
        total_likelihood[kb] = log_likelihood
    # choose key with best likelihood
    answer.append(np.argmax(total_likelihood))

print(",".join(map(str, answer)))