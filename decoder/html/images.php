<?php
include "header.inc.php";
?>
<script type="text/javascript">

lastrxtime = <?=time()-$range?>;
range = <?=$range?>;
call = '<?=$_GET['call']?>';

function loadImages() {
	updateData();
	setInterval(updateData, 1000);
}
</script>
<script type="text/javascript" src="script.js"></script>
</head>
<body onload="loadImages()">

<?php
include "sidebar.inc.php";
include "topbar.inc.php";
?>

<div id="images" class="data"></div>

</body>
</html>
<?php
include "footer.inc.php";
?>

