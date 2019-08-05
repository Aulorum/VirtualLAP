IDEs:
 - CLion for Desktop (free for students after registration, <https://www.jetbrains.com/shop/download/CL/2019100>)
   - Requires [MinGW Compiler]()
     - use 8.1.0 x86_64 Posix SEH
     - install in a path without spaces, even if installer suggests Program Files
 - Android Studio for Android
 
 to clone
 ```
 git clone --recurse-submodules -j8 https://github.com/hexd0t/VirtuaLAP.git
 ```

Folder structure:
VirtuaLAP (select this as project folder for CLion)
- Android: Android specific code (select this as project folder for Android Studio
- Desktop: Desktop specific code
- Global: Platform independent code
- Data: Platform independent resources (textures, model files, shader source)
