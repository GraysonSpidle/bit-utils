# bit-utils
This is a programming exercise that aimed to help me understand C better.

This is targeted towards C++11 and C++17 language standards.

The C++11 version is just a namespace with functions that, admittingly, have shitloads of parameters, but it doesn't require you to know the size of memory blocks at compile time.

The C++17 version adds a generic class that aims to make the number of parameters for each function much shorter by making them template arguments. This does have the drawback of requiring you to know the sizes of memory blocks at compile time, but on the upside, it uses SFINAE so we don't have to do checks we don't need to at runtime.

## Installation Notes

IF YOU'RE USING VC++ THEN ADD THE COMPILE COMMAND LINE ARGUMENT `/Zc:__cplusplus`.

While I do refer to a C++11 and C++17 version, that does not mean the C++11 version is inferior to its "newer" counterpart. In my opinion it's not really.
If you don't want the C++17 stuff in there, then all you have to do is chuck any file into this https://regex101.com/r/4DdvON/2 and copy the substitution result. It's really that easy. Visual Studio sucks and it doesn't allow you to do multiline regex so this is the next best thing.

The C++17 version isn't tested as extensively as the C++11 version because you need to know the size of stuff at compile time and I'm not about to enumerate every single permutation of bit arrays just to test it. 

If, for whatever reason, you decide you want to use this I just want to warn you that **this is not dummy proof and if you do decide to use it, you gotta know what you're doing.** 

This is for very niche uses and I don't think everyone should use it. While making this, I had a number of times when I could've done serious damage to my computer if it wasn't for the heap corruption error that msvc gives you.

Installation is as easy as drag and drop the `bit-utils` folder into your include folder.
