Local Alignment with Affine Gap Penalty

Problem

Given: Two protein strings s and t in FASTA format (each having length at most 10,000 aa).

Return: The maximum local alignment score of s and t, followed by substrings r and u of s and t, respectively, that correspond to the optimal local alignment of s and t. Use:

The BLOSUM62 scoring matrix.
Gap opening penalty equal to 11.
Gap extension penalty equal to 1.
If multiple solutions exist, then you may output any one.