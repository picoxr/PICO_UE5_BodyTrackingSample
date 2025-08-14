//
// Created by Nico on 2024/3/25.
//

#pragma once
namespace ps_base {
#define PS_MAX(a, b) ((a) > (b) ? (a) : (b))
#define PS_MAX3(a, b, c) PS_MAX(PS_MAX(a, b), c)
#define PS_MIN(a, b) ((a) > (b) ? (b) : (a))
#define PS_MIN3(a, b, c) PS_MIN(PS_MIN(a, b), c)
}  // namespace ps_base