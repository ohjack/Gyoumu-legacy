<?php
require_once 'PHPTAL.php';

$p = $_POST;

$tpl = new PHPTAL('tpl_' . $p['account'] . '.xhtml');
$tpl->title = $p['title'];
$tpl->pinfo = explode("\n", $p['pinfo']);
$tpl->pics = json_decode(urldecode($p['pics']));
$tpl->sku = $p['sku'];

//examine hideYear & hideRemark
$tpl->hideYear = true;
$tpl->hideRemark = true;

$fitment = json_decode(urldecode($p['fit']));
//format fitment
for($c = 0; $c < sizeof($fitment); $c++){
    if($fitment[$c]->year_from){
        $tpl->hideYear = false;        
        
        if($fitment[$c]->year_to)
            $fitment[$c]->year = $fitment[$c]->year_from . ' ~ ' . $fitment[$c]->year_to;
        else
            $fitment[$c]->year = $fitment[$c]->year_from . ' UP';
    }
        
    if($fitment[$c]->remark != ''){
        $tpl->hideRemark = false;
    }
}
$tpl->fitment = $fitment;

/*
echo '<pre>';
print_r($tpl);
echo '</pre>';
*/

try {
    echo $tpl->execute();
}
catch (Exception $e){
    echo $e;
}

?>
