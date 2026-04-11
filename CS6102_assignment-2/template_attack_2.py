import numpy as np
import matplotlib.pyplot as plt
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

def find_best_window(trace, window_size=4500):
    # Sliding window sum using convolution (fast)
    trace = np.trim_zeros(trace)
    window = np.ones(window_size)
    scores = np.convolve(trace ** 2, window, mode='valid')

    start = np.argmin(scores)
    end = start + window_size

    return start, end

def align_traces(traces): 
    result = []
    for trace in traces:
        start, end = find_best_window(trace)
        result.append(trace[start:end])
    return np.array(result)

# Importing the Profiling and Attack traces [Do not modify]
# -----------------------------------------------------
data = np.load("profiling_traces.npz")

trace_array = align_traces(data["traces"])
textin_array = data["textin"]
textout_array = data["textout"]
key_array = data["keys"]

data = np.load(f"attack_traces_team_{team_number}.npz") 

attack_traces = align_traces(data["traces"])
attack_textins = data["textin"]
attack_textouts = data["textout"]
# -----------------------------------------------------

# plot the mean trace array
plt.figure(figsize=(12, 4))
plt.plot(trace_array.mean(axis=0), linewidth=0.8)   
plt.title("Mean Profiling Trace (Aligned)")
plt.xlabel("Sample Index")
plt.ylabel("Amplitude")
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.show()

# (num_traces, 16)
sbox_outputs = sbox[textin_array ^ key_array]
hamming_weights = np.bitwise_count(sbox_outputs)

# should be called for each sbox separately
# poi power values for each trace 
# X -> (num_traces, win), hw -> (num_traces,)
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

# calculate log likelihood of poi trace values given class stats
def compute_log_likelihood(attack_trace_pois, mean, cov):
    d = len(mean)
    diff = attack_trace_pois - mean
    inv_cov = np.linalg.inv(cov)
    log_det_cov = np.log(np.linalg.det(cov) + 1e-12)
    log_likelihood = -0.5 * (diff @ inv_cov @ diff.T + log_det_cov)
    return log_likelihood

# finding PoIs
# X : (num_traces, num_samples) , y : either intermediate value, or hamming weight of intermediate value (num_traces,1)
def get_best_window(X:np.ndarray, y:np.ndarray, win=10):
    # Center the data
    X_centered = X - X.mean(axis=0, keepdims=True)
    y_centered = y - y.mean(axis=0, keepdims=True)

    # Compute numerator: covariance with y
    # print(X_centered[:, None].shape, y_centered[None, :].shape)
    numerator = X_centered.T @ y_centered   # shape: (num_samples, 1)

    # Compute denominator: std devs
    X_std = np.sqrt((X_centered ** 2).sum(axis=0))
    y_std = np.sqrt((y_centered ** 2).sum(axis=0))

    correlations = numerator / (X_std * y_std)

    # Handle divide-by-zero (if any constant columns)
    correlations = np.abs(np.nan_to_num(correlations))

    # aggregate correlation in windows of size win and find the best window
    num_windows = len(correlations) - win + 1
    window_sums = np.array([np.sum(correlations[i:i+win]) for i in range(num_windows)])
    best_window_start = np.argmax(window_sums)

    return best_window_start, correlations[best_window_start:best_window_start+win]

answer = []

win = 5
for byte in range(16):
    best_window_start, corr = get_best_window(trace_array, hamming_weights[:, byte], win)

    X_pois = trace_array[:, best_window_start:best_window_start+win]

    means, covs = compute_class_stats(X_pois, hamming_weights[:, byte])

    # pranay plan

    total_likelihood = np.zeros(256)

    # loop over keys
    for kb in range(256):
        # loop over attack traces
        for atk_trace, atk_textin in zip(attack_traces, attack_textins):
            # calculate attack trace's PoI values
            atk_pois = atk_trace[best_window_start:best_window_start+win]

            # calculate class of hypothetical interm value for this key guess
            hyp_sbox_out = sbox[atk_textin[byte] ^ kb]
            hyp_hw = np.bitwise_count(hyp_sbox_out)
            
            # add the likelihood of key to total likelihood for that key
            total_likelihood[kb] += compute_log_likelihood(atk_pois, means[hyp_hw], covs[hyp_hw])
    # choose key with best likelihood
    answer.append(np.argmax(total_likelihood))

print(",".join(map(str, answer)))