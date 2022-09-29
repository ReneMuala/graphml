<!-- 
    Author: Rene Descartes Muala
 -->
<graphml version="latest" engine="cairo" mode="relative">

    <gradient id="grad" type="linear" begin="0,0" end="100%,1">
        <colorstop step="0.0" color="0.1,0.15,0.24" />
        <colorstop step="0.5" color="0.29,0.48,0.94" />
        <colorstop step="1.0" color="0.51,0.76,0.96" />
    </gradient>

    <gradient id="grad2" type="radial" begin="0.5,0.5,0.0" end="0.5,0.5,0.4">
        <colorstop step="0.0" color="0.1,1.0,0.24" />
        <colorstop step="0.5" color="0.29,0.48,0.94" />
        <colorstop step="1.0" color="0.99,0.76,0.96" />
    </gradient>

    <path id="path1">
        <move x="0" y="0" />
        <line x="0.5" y="0.5" />

    </path>

    <path id="path2">
        <curve begin="0,0.5" middle="0.5, 0.9" end="1,0.5"/>
        <curve begin="1,0.5 " middle="0.5,0" end="0,0.5"/>
    </path>

    <path id="path1">
        <!-- <rect x ="0" y="0" w="0.2" h="0.2" /> -->
        <move x="0" y="0" />
        <line x="1" y="1" />
    </path>
    <path id="arc">
        <arc center = "[2]{0.5}" radius="0.3" begin="0" end="*((@pi),2)"/>
    </path>

     <gradient id="grad" type="linear" begin="7,0" end="1,1">
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


<graphml>

    <img w="400" h="600">
        <apply path="arc">
            <fill color= "0,0,0"/>
        </apply>
        <textstr x="0" y="0" font="Arial">
            <fill color= "1,1,1"/>
        </textstr>
        <!-- <rect x="0.1" y="0.1" h="1.28"  w ="0.8">
            <fill gradient="grad" />
            <rect x="0.5" y="0.5" h="0.9" w ="0.2">
                <fill color="1,1,1,0.1" />
            </rect>
            <arc center="0.5,0.5" radius="0.3" begin="0" end="*((@pi),2)">
                <stroke gradient="grad2" width="0.1"/>
                <arc center="0.5,0.5" radius="0.2" begin="0" end="*((@pi),2)">
                    <fill gradient="grad2" />
                </arc>
            </arc>
        </rect> -->
        
        <!-- <rect x="0" y="0" w="1" h="1.5">
            <fill color="0,0,0" />
            <rect x="0.25" y="0.25" w="0.5" h="0.5">
                <arc center="0.25,0.5" radius="(r=0.12)(r)" begin="0.0" end="*((@pi),2)">
                    <fill gradient="grad2"/>
                </arc>
                <arc center="0.75,0.5" radius="(r)" begin="0.0" end="*((@pi),2)">
                    <fill gradient="grad2"/>
                </arc>
                <arc center="0.5, 0.25" radius="(r)" begin="0.0" end="*((@pi),2)">
                    <fill gradient="grad2"/>
                </arc>
                <arc center="0.5,+(0.5,0.25)" radius="(r)" begin="0.0" end="*((@pi),2)">
                    <fill gradient="grad2"/>
                </arc>
                <arc center="0.5,0.5)" radius="(r)" begin="0.0" end="*((@pi),2)">
                    <fill gradient="grad"/>
                </arc>
            </rect>
        </rect> -->
        
        <rect x="0" y="0" w = "0.3" h ="0.3">
            <fill gradient="grad"/>
        </rect>
        <apply path="path1">
            <stroke gradient="grad2" width="0.1"/>
            <rect x="0" y="0" w="1" h="1">
                <stroke color="1,1,1" width="0.1"/>
            </rect>
            <apply path="path2">
                <fill gradient="grad2"/>
            </apply>
        </apply>
        <export filename="helloworld.png" />
    </img>

</graphml>