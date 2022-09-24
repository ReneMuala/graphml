<!-- 
    Author: Rene Descartes Muala
 -->
<graphml version="latest" engine="cairo" mode="relative">

     <gradient id="grad" type="linear" begin="0,0" end="1,1">
        <colorstop step="0.0" color="1,0,0" />
        <colorstop step="div(1,2)" color="0,1,0" />
        <colorstop step="1.0" color="0,0,1" />
    </gradient>

    <font id="myFont" family="Arial" size="14" decoration="underline" />

    <!-- <pattern id="id"> </pattern> -->

    <img w="set(w,8900)(w)" h="set(h,900)(h)">
        <rect x="0.1" y = "0.2" w="0.5" h="0.5">
        </rect>
    </img>

    <img id="helloworld" w="(m=3)(m)" h="400">
        <rect x="0" y="0" w="div(40, 400)" h="div(40,400)">
            <fill color="rgb(0,0,0)" />
            <stroke color="#fff" width="1" />
        </rect>
        <circle center="0.5,0.5" radius="0.1">
            <fill gradient="grad" />
        </circle>
        <arc center="0.5,0.5" radius="0.1" begin="0.0" end="mul(2,(@pi))">
            <fill color="0,1,1" />
        </arc>
        <text x="300" y="400" value="Ola mundo" font="myFont">
            <fill color="rgb(0,32,120)" />
        </text>
        <pixel x="10" y="10">
            <fill color="rgb(244,0,255)" />
        </pixel>
        <export filename="helloworld.png" />
    </img>
</graphml>