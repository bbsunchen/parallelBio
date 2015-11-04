Global Alignment with Scoring Matrix
-------------------------------------
Problem

To penalize symbol substitutions differently depending on which two symbols are involved in the substitution, we obtain a scoring matrix S in which Si,j represents the (negative) score assigned to a substitution of the ith symbol of our alphabet A with the jth symbol of A.

A gap penalty is the component deducted from alignment score due to the presence of a gap. A gap penalty may be a function of the length of the gap; for example, a linear gap penalty is a constant g such that each inserted or deleted symbol is charged g; as a result, the cost of a gap of length L is equal to gL.

Given: Two protein strings s and t in FASTA format (each of length at most 1000 aa).

Return: The maximum alignment score between s and t. Use:

The BLOSUM62 scoring matrix.
Linear gap penalty equal to 5 (i.e., a cost of -5 is assessed for each gap symbol).

Align Two Strings Using Linear Space
-------------------------------------

Find the highest-scoring alignment between two strings using a scoring matrix in linear space.

Given: Two long amino acid strings (of length approximately 10,000).

Return: The maximum alignment score of these strings, followed by an alignment achieving this maximum score. Use the BLOSUM62 scoring matrix and indel penalty σ = 5.