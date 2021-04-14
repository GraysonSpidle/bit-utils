# bit-utils
This is a programming exercise that aimed to help me understand C better.

This is targeted towards C++17 language standard.

This aims to make functions that can read/write bits as if they were in an array. Technically they are, but it isn't like a conventional array.

I first went with a namespace apparoach and thought it was okay.
Then I went with a generic class with static functions and I like it a lot more because I can use SFINAE to validate some things so as to minimize the amount of accidental destruction done.

`BitUtils.h` isn't really the final product. If you, for whatever reason, want to use this stuff then use `BitUtils2.h`
