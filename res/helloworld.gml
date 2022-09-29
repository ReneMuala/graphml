<graphml>
    <gradient id="grad1" type="linear" begin="0%, 0%" end="100%,100%">
        <colorstop step="0%" color="rgb(255,90,0)" />
        <colorstop step="50%" color="rgb(255,90,255)" />
        <colorstop step="100%" color="rgb(0,90,255)" />
    </gradient>
    <gradient id="grad2" type="radial" begin="0% , 0% , 10%" end="100%,100%,90%">
        <colorstop step="0%" color="rgb(25,90,0)" />
        <colorstop step="50%" color="rgb(255,90,255)" />
        <colorstop step="100%" color="rgb(0,90,255)" />
    </gradient>
    <gradient id="grad3" type="radial" begin="50% , 50% , 1%" end="50%,50%,30%">
        <colorstop step="20%" color="rgb(105,40,105)" />
        <colorstop step="100%" color="rgb(0,40,105)" />
    </gradient>
    <path id="caminho1">
        <move x="0%" y="0%"/>
        <line x="50%" y="50%"/>
        <line x="0%" y="100%"/>
    </path>

    <path id="caminho2">
        <move x="100%" y="0%"/>
        <line x="50%" y="50%"/>
        <line x="100%" y="100%"/>
    </path>
    <import id= "image1" filename="helloworld2.png"/>
    <img id="img1" w="800" h="800">
        <paint gradient="grad3"/>
        <translate x="50%" y="50%">
        <scale x="2" y="2">
        <translate x="-(50%)" y="-(50%)">
            <arc center="50%,50%" radius="50%" begin="0" end="rad(360)">
            <fill gradient="grad2" />
                <apply path="caminho1">
                <fill gradient="grad1"/>
            </apply>
            <apply path="caminho2">
                <fill gradient="grad1"/>
            </apply>
        </arc>
            </translate>
        </scale>
        </translate>
        <scale x="0.5" y="0.5">
        <translate x="-(50%)" y="-(50%)">
        <arc center="50%,50%" radius="50%" begin="0" end="rad(360)">
            <fill gradient="grad2" />
                <apply path="caminho1">
                <fill gradient="grad1"/>
            </apply>
            <apply path="caminho2">
                <fill gradient="grad1"/>
            </apply>
        </arc>
        </translate>
        </scale>
        <translate x="(50%)" y="-(50%)">
        <arc center="50%,50%" radius="50%" begin="0" end="rad(360)">
            <fill gradient="grad2" />
                <apply path="caminho1">
                <fill gradient="grad1"/>
            </apply>
            <apply path="caminho2">
                <fill gradient="grad1"/>
            </apply>
        </arc>
        </translate>
        <translate x="-(50%)" y="(50%)">
        <arc center="50%,50%" radius="50%" begin="0" end="rad(360)">
            <fill gradient="grad2" />
                <apply path="caminho1">
                <fill gradient="grad1"/>
            </apply>
            <apply path="caminho2">
                <fill gradient="grad1"/>
            </apply>
        </arc>
        </translate>
        <export filename="helloworld.png"/>
    </img>
    <img w="800" h="800">
        <arc center="50%,50%" radius="40%" begin="0" end="rad(360)">
            <stroke width="20%" image="img1"/>
        </arc>
        <scale x="0.8" y="0.8">
            <arc center="62%,60%" radius="20%" begin="0" end="rad(360)">
                <fill image="image1"/>
            </arc>
        </scale>
        <export filename="helloworld1.png"/>
    </img>
</graphml>

