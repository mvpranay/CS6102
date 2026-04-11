import numpy as np
from scipy.linalg import eigh

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


def align_traces(traces, reference, max_shift=1000):
    """Cross-correlation alignment of traces against a reference waveform."""
    aligned = np.zeros_like(traces)
    ref_c = reference - reference.mean()
    for i, trace in enumerate(traces):
        corr = np.correlate(trace - trace.mean(), ref_c, mode='full')
        center = len(reference) - 1
        window = corr[center - max_shift: center + max_shift + 1]
        best_shift = np.argmax(window) - max_shift
        if best_shift > 0:
            aligned[i, best_shift:] = trace[:-best_shift]
        elif best_shift < 0:
            aligned[i, :best_shift] = trace[-best_shift:]
        else:
            aligned[i] = trace
    return aligned


# Importing the Profiling and Attack traces [Do not modify]
# -----------------------------------------------------
data = np.load("profiling_traces.npz")

trace_array  = data["traces"]    # (5000, 6000)
# trace_array = align_traces(trace_array, trace_array.mean(axis=0))
textin_array = data["textin"]    # (5000, 16)
key_array    = data["keys"]      # (5000, 16)

data = np.load(f"attack_traces_team_{team_number}.npz")

attack_traces  = align_traces(data["traces"], trace_array.mean(axis=0))
attack_textins = data["textin"]  # (100, 16)
# -----------------------------------------------------

# plot the first 5 aligned traces to verify
import matplotlib.pyplot as plt
plt.figure(figsize=(12, 6))
for i in range(79,79+1):
    plt.plot(attack_traces[i], linewidth=0.8, label=f'Trace {i}')

plt.title("First 5 Aligned Profiling Traces")
plt.xlabel("Sample Index")
plt.ylabel("Amplitude")
plt.grid(True, alpha=0.3)
plt.legend()
plt.tight_layout()
plt.show()

exit(0)

# Profiling: intermediate values for all 16 key bytes
sbox_outputs    = sbox[textin_array ^ key_array]     # (5000, 16)
hamming_weights = np.bitwise_count(sbox_outputs)     # (5000, 16)


def ftest_snr(X, labels, num_classes):
    """
    F-test statistic per sample point (Choudary & Kuhn 2014, eq. 7).
    Measures how much inter-class mean variation exceeds intra-class variance.
    """
    n = X.shape[0]
    grand_mean = X.mean(0)
    between = np.zeros(X.shape[1])
    within  = np.zeros(X.shape[1])
    for k in range(num_classes):
        idx = labels == k
        nk  = idx.sum()
        if nk < 2:
            continue
        mk      = X[idx].mean(0)
        between += nk * (mk - grand_mean) ** 2
        within  += ((X[idx] - mk) ** 2).sum(0)
    between /= (num_classes - 1)
    within  /= (n - num_classes)
    return between / (within + 1e-12)


# ── Attack parameters ──────────────────────────────────────────────────────────
n_a       = len(attack_traces)   # 100 attack traces
trace_idx = np.arange(n_a)

num_hw  = 9     # HW classes 0..8
k_pois  = 200   # initial POI pool selected by F-test SNR
m_lda   = num_hw - 1  # max LDA directions for 9 classes = 8

key_output = []

for byte in range(16):
    hw_prof = hamming_weights[:, byte]   # (5000,) — HW labels 0..8

    # ── 1. F-test SNR: select top k_pois samples ──────────────────────────────
    snr      = ftest_snr(trace_array, hw_prof, num_hw)
    poi_cols = np.sort(np.argsort(snr)[::-1][:k_pois])
    Xp = trace_array[:, poi_cols]         # (5000, k_pois)
    Xa = attack_traces[:, poi_cols]       # (n_a,  k_pois)

    n, d = Xp.shape

    # ── 2. Per-class means in raw POI space ───────────────────────────────────
    class_means = np.zeros((num_hw, d))
    for k in range(num_hw):
        idx = hw_prof == k
        if idx.any():
            class_means[k] = Xp[idx].mean(0)

    grand_mean = Xp.mean(0)

    # ── 3. Between-groups matrix B (paper eq. 8) ──────────────────────────────
    B = np.zeros((d, d))
    for k in range(num_hw):
        nk = int((hw_prof == k).sum())
        if nk == 0:
            continue
        diff = class_means[k] - grand_mean
        B += nk * np.outer(diff, diff)

    # ── 4. Pooled within-class covariance S_pooled (paper eq. 21) ─────────────
    residuals = Xp - class_means[hw_prof]           # row i → residual from its class mean
    S_pooled  = (residuals.T @ residuals) / (n - num_hw)
    S_pooled += 1e-6 * np.eye(d)

    # ── 5. LDA: solve B u = λ S_pooled u (paper eq. 14-16) ───────────────────
    # scipy eigh(B, S_pooled) normalises columns of U so that U.T @ S_pooled @ U = I
    # → in the projected space, S_pooled_lda = I (pooled cov becomes identity)
    # We only need the top m_lda eigenvectors (largest eigenvalues).
    _, U = eigh(B, S_pooled, subset_by_index=[d - m_lda, d - 1])
    U = U[:, ::-1]   # descending eigenvalue order → (d, m_lda)

    # ── 6. Project profiling and attack traces to LDA space ───────────────────
    Xp_lda = Xp @ U   # (5000, m_lda)
    Xa_lda = Xa @ U   # (n_a,  m_lda)

    # ── 7. Class means in LDA space ───────────────────────────────────────────
    lda_means = np.zeros((num_hw, m_lda))
    for k in range(num_hw):
        idx = hw_prof == k
        if idx.any():
            lda_means[k] = Xp_lda[idx].mean(0)

    # ── 8. d_LINEAR^joint discriminant (paper eq. 29) ─────────────────────────
    # Because S_pooled_lda = I, d_LINEAR simplifies to:
    #   d(k | x_i) = μ_k · x_i  −  ½ ‖μ_k‖²
    # Summing over all n_a attack traces for each key guess:
    #   score(kb) = Σ_i [ μ_{hw_i} · x_i  −  ½ ‖μ_{hw_i}‖² ]
    MX = lda_means @ Xa_lda.T         # (num_hw, n_a) — MX[k,i] = μ_k · x_i
    C  = 0.5 * (lda_means ** 2).sum(1) # (num_hw,)    — C[k] = ½‖μ_k‖²

    scores = np.empty(256)
    for kb in range(256):
        hyp_hw     = np.bitwise_count(sbox[attack_textins[:, byte] ^ kb])  # (n_a,)
        scores[kb] = MX[hyp_hw, trace_idx].sum() - C[hyp_hw].sum()

    key_output.append(int(np.argmax(scores)))

print(",".join(map(str, key_output)))
