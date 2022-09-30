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