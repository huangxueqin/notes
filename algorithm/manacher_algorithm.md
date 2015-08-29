Manacher's algorithm can solve "Longest Palindromic Substring" problem in linear time.
    The mathematical principle in manacher's method is:
        In a palindrome string of which length is 2 * n + 1, so the center of the string is
    the char of which the index is n. Let's assume p < n and q = 2*n-p. We can proof that
    the length of palindromic substring centered with q is greater than or equal to that
    of the corresponding index of p.
    Proof:
        let whole string named "s".
        for each index t, we have s[t] = s[2*n-t]
        if s[t] = s[2*p-t], we can get
        s[2*n-t] = s[2*p-t] => s[2n-p+(p-t)] = s[p+(p-t)]
                            => s[q+(p-t)] = s[2*n-p-(p-t)]
                            => s[q+(p-t)] = s[q-(p-t)]
        so the conclusion is correct.

    For a arbitrary string, there may exist some palindromic substring of which length is
    2*n. So for every string, a pre-processing is needed:
        "abcdefg" => "#a#b#c#d#e#f#g#"

        The code of manacher's algorithm is in ../code/manacher.c
