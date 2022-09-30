# GRAPHML

A new way to create images

```
<graphml>
   <img w="400" h="400">
       <paint color="rgb(0,0,0)" />
       <rect x="10%" y="10%" w="80%" h="80%">
           <fill color="rgb(50, 120, 255)"/>
           <arc center="90%,90%" radius="60%"
           begin="0" end="rad(360)">
               <fill color="rgb(255,255,255)"/>
           </arc>
        </rect>
       <export filename="image.png"/>
   </img>
</graphml>
```

![image.png](./image.png)

## building

1. install clang, gnu make, libxml2 and libcairo (doxygen is optional).
2. __(optional)__ create an alias or symbolic link to "libxml2/libxml" named "libxml" at the include directory ("usr/include/" on most linux distros, by using "ln -s libxml2/libxml libxml") in case of having libxml2 header file errors...
3. compile by running __make__ at the project's root folder.

## using

The graphml binary wil be compiled and saved to de [build/](/build/) folder.

## documentation

___cheat sheet___ (https://github.com/renemuala/graphml-cheat-sheet)

### some outputs

1. Arcs
![arcs](./helloworld.png)

2. Smile
![face](./helloworld2.png)

3. arc and smile composition
![arc&face](/helloworld1.png)