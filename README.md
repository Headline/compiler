# Language X [![Build status](https://ci.appveyor.com/api/projects/status/xe77ex6oxl497xjm/branch/master?svg=true)](https://ci.appveyor.com/project/Headline22/compiler/branch/master)

## Description
A basic compiler for a scripting language named Language X. This language is C-like, and is designed to be easy to learn for beginners. While it's functionality is currently limited, it will have the following properties.

- Not OOP, meaning classes and structs will never be supported
- Like this projects inspiriation, it is typeless. The only type is the 32-bit integer `int`
- This project will always be backwards compatable, meaning old scripts will never break

## Usage
Beauty is in the eye of the beholder and this project is no exception to that. While crude, this project's is intended to run *very* quickly, with performance and ease-of-use as it's first and second priorities. As development continues, more documentation will be created & guides will be released with instruction on how to embed the language into your use-case.

## Development
The first focus is on the design of the compiler, whose construction will pause when code generation is on the horizon. Then, the VM will need a framework for the compiler to generate code for it.

This project will not be rushed and periods of stale development may occur. If you'd like to contribute to this compiler and help out you're more than welcome, but I request that you collaborate heavily with me or your pull requests may go stale or be declined (something I hate doing). 
