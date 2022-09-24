<graphml>

    <gradient id="grad" type="linear" begin="0,0" end="1,1">
        <colorstop step="0.0" color="0.1,0.15,0.24" />
        <colorstop step="0.5" color="0.29,0.48,0.94" />
        <colorstop step="1.0" color="0.51,0.76,0.96" />
    </gradient>

    <gradient id="grad2" type="radial" begin="0.5,0.5,0.0" end="0.5,0.5,0.4">
        <colorstop step="0.0" color="0.1,1.0,0.24" />
        <colorstop step="0.5" color="0.29,0.48,0.94" />
        <colorstop step="1.0" color="0.99,0.76,0.96" />
    </gradient>

    <img w="400" h="600">
        <rect x="0.1" y="0.1" w="0.5" h="0.5" >
        <fill gradient="grad2"/>
        </rect>
       
        <export filename="helloworld.png" />
    </img>

</graphml>