bias = 255


def parse_binary_float(binary_str):
    # Parse sign, exponent, and significand from binary string
    sign = int(binary_str[0], 2)
    exponent = int(binary_str[1:10], 2)
    significand = int(binary_str[10:], 2)

    value = sign * (1 + significand/1e23) * 2 ** (exponent - bias)
    print(value)

    return sign, exponent, significand


def to_binary_format(sign, exponent, significand):
    # Convert sign, exponent, and significand back to binary format
    sign_bit = format(sign, '1b')
    exponent_bits = format(exponent, '09b')
    significand_bits = format(significand, '022b')
    return sign_bit + exponent_bits + significand_bits


def add_floating_points(bin_num1, bin_num2):
    # Parse both binary numbers
    sign1, exp1, sig1 = parse_binary_float(bin_num1)
    sign2, exp2, sig2 = parse_binary_float(bin_num2)

    # Adjust exponents by shifting the significand of the number with the smaller exponent
    if exp1 > exp2:
        shift_amount = exp1 - exp2
        sig2 >>= shift_amount  # Shift significand of bin_num2
        result_exponent = exp1
    else:
        shift_amount = exp2 - exp1
        sig1 >>= shift_amount  # Shift significand of bin_num1
        result_exponent = exp2

    # Determine the result sign and add or subtract significands based on the signs
    if sign1 == sign2:
        result_sign = sign1
        result_significand = sig1 + sig2
    else:
        if sig1 > sig2:
            result_sign = sign1
            result_significand = sig1 - sig2
        else:
            result_sign = sign2
            result_significand = sig2 - sig1

    # Normalize result if there's a carry in the significand
    if result_significand >= (1 << 23):  # If result needs one extra bit
        result_significand >>= 1
        result_exponent += 1

    overflow = 0
    underflow = 1

    # Check for overflow and underflow
    if result_exponent >= 0b111111111:
        overflow = 1
    elif result_exponent <= 0:
        underflow = 1

    # Ensure result_significand fits within 22 bits (remove hidden 1 if needed)
    result_significand &= (1 << 22) - 1

    # Convert result to binary format
    result_binary = to_binary_format(
        result_sign, result_exponent, result_significand)

    return result_binary, overflow, underflow
