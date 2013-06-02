/** @mainpage C++11 Proton Library
*
* @authors <a href="http://lenx.100871.net">Lenx Tao Wei</a> (lenx.wei at gmail.com)
*
* @section intro Introduction
* Proton is a library to provide Python-like interfaces for C++11,
* which try to make porting from Python to C++11 easier,
* and make programming in C++11 more convenient :)
*
* The main git repository is at https://github.com/LenxWei/libproton.
*
* @section install Install
* Download site:
* - http://code.google.com/p/libproton/downloads/list
* - https://github.com/LenxWei/libproton/downloads
*
* Need a compiler supporting c++11, such as gcc 4.7, clang 3.2 or higher.<br>
* Install <a href="http://www.boost.org/">Boost C++ Library</a> 1.48 or higher first.
*
* After that, just standard "./configure; make; make check; make install"
*
* @section modules Modules
* You can find documents about modules <a href="modules.html">here</a>.
* <hr>
* @todo add tutorial
* @todo add weak_
* @todo 1.2 milestone
* @todo add os module
* @todo add socket module(?)
* @todo add server/client module
* @todo add regex module
* @todo 1.3 milestone
* @todo add threading
* @todo 1.4 milestone
* @todo add gc support
* @todo 1.6 milestone
*
* @defgroup ref Smart reference
* Provide basic reference support for proton.
* @{
*  @defgroup ref_ ref_
*  Core reference template supporting interface through inheritance/multiple inheritance.
*
*  @defgroup functor func_
*  General functor interface template.
*
* @}
*
* @defgroup stl Containers
* Repack stl containers in new templates following python's habits.
* @{
*  @defgroup seq common
*  common container operations
*
*  @defgroup str str
*  like string in python.
*
*  @defgroup vector_ vector_
*  like list in python, better for small sequences
*
*  @defgroup deque_ deque_
*  like list in python, better for long sequences.
*
*  @defgroup map_ map_
*  like dict in python.
*
*  @defgroup set_ set_
*  like set in python.
*
*  @defgroup tuple tuple
*  like tuple in python.
*
* @}
*
* @defgroup util Utilities
* @{
*
*  @defgroup getopt getopt
*  get options
*
*  @defgroup debug Debug utilities
*  Macros, global variables, classes for debug.
*
*  @defgroup pool Smart allocator
*  A high-performance and smart allocator.
*
* @}
*
*/
