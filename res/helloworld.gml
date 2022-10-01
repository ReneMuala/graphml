<graphml>
<gradient id="grad3" type="radial" begin="50% , 50% , 1%" end="50%,50%,30%">
        <colorstop step="20%" color="rgb(105,40,105)" />
        <colorstop step="100%" color="rgb(0,40,105)" />
    </gradient>
    <import id="img" filename="helloworld.png" /> 
   <img w="800" h="800">
       <!-- <paint color="rgb(0,0,0)" />
       <rect x="10%" y="10%" w="80%" h="80%">
           <fill color="rgb(50, 120, 255)"/>
           <arc center="90%,90%" radius="60%"
           begin="0" end="rad(360)">
               <fill color="rgb(255,255,255)"/>
           </arc>
        </rect> -->
        <paint color="[3]{1}"/>
        <rect x="0" y="0" w="10%" h="10%">
            <fill color="rgb(255,0,255)"
        </rect>
        <textstr value="GRÄÇPHML" x="0%" y="0%" font="mono" size="10%" weight="bold">
                        <!-- <fill image="img"/> -->
            <fill color="[3]{0}"/> 
        </textstr>

        <!-- <rect x="80%" y="0" w="40%" h="100%">
            <paint color="rgb(240,240,240)"/>
            <translate x="95%" y="95%">
                <rotate rad="neg(rad(90))">
                    <textstr value="GRAPHML" x="10%" y="10%" font="mono" size="50%" weight="bold">
                        <fill image="img"/>
                        <fill color="[3]{0}"/> 
                    </textstr>
                </rotate>
            </translate>
        </rect> -->

       <export filename="image.png"/>
   </img>
</graphml>