<!-- 
    Author: Rene Descartes Muala
 -->
<graphml version="latest" engine="cairo">
    <gradient id="grad" type="linear" begin="relative(0,0)" end="relative(1,1)">
        <colorstop step="0.0" color="#f00" />
        <colorstop step="div(1,2)" color="#0f0" />
        <colorstop step="1.0" color="#00f" />
    </gradient>

    <font id="myFont" family="Arial" size="14" decoration="underline" />

    <!-- <pattern id="id"> </pattern> -->

    <img w="mul(add(100,200),2)" h="400">
        <fill colo="rgb(0,0,0)" />
        <rect x="100" y="100" w="50" h="50">
            <fill color="#ff0" />
        </rect>
    </img>

    <img id="helloworld" w="(m<-3)(m)" h="400">
        <rect x="0" y="0" w="div(40, 400)" h="40">
            <fill color="rgb(0,0,0)" />
            <stroke color="#fff" width="1" />
        </rect>
        <circle center="200,200" radius="100">
            <fill gradient="grad" deg="90" />
        </circle>
        <text x="300" y="400" value="Ola mundo" font="myFont">
            <fill color="rgb(0,32,120)" />
        </text>
        <pixel x="10" y="10">
            <fill color="rgb(244,0,255)" />
        </pixel>
        <export filename="helloworld" format="png" />
    </img>
</graphml>