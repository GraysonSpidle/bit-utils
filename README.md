# bit-utils
This is a programming exercise that aimed to help me understand C better.

This is targeted towards C++11 and C++17 language standards.

The C++11 version is just a namespace with functions that, admittingly, have shitloads of parameters, but it doesn't require you to know the size of memory blocks at compile time.
It is also worth noting that this version is less forgiving with wrong values for variables.

The C++17 version adds a generic class that aims to make the number of parameters for each function much shorter by making them template arguments. This does have the drawback of requiring you to know the sizes of memory blocks at compile time, but on the upside, it uses SFINAE so we don't have to do checks we don't need to at runtime.

If, for whatever reason, you decide you want to use this I just want to warn you that **this is not dummy proof and if you do decide to use it, you gotta know what you're doing.** 

This is for very niche uses and I don't think everyone should use it. While making this, I had a number of times when I could've done serious damage to my computer if it wasn't for the heap corruption error that msvc gives you.
