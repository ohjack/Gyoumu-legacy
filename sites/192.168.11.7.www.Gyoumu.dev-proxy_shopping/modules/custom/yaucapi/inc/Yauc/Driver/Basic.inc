<?php
namespace Yauc\Driver;

class Basic extends \AbstractYaucDriver
{
  public function getVarExp($name)
  {
    return sprintf('javascript{storedVars["%s"]}', $name);
  }

  public function runScript($code)
  {
    $this->doCommand('runScript', array(sprintf('javascript{%s}', $code)));
  }

  public function typeLongText($loc, $text)
  {
    $this->runScript('storedVars["longtext"] = "";');
    preg_match_all('/[\s\S]{1,1000}>/u', $text, $m);

    foreach($m[0] as $chunk){
      $cmd = "storedVars['longtext'] = storedVars['longtext'].concat('" . $chunk . "');";
      $this->runScript($cmd);
    }
    $this->doCommand('type', array($loc, $this->getVarExp('longtext')));;
  }

  public function selectLastOption($xpSel)
  {
    $valOpt = $this->getString('getValue', array("$xpSel/option[last()]"));
    $this->doCommand('select', array($xpSel, 'value=' . $valOpt));
  }

  public function getString($cmd, $arg)
  {
    return parent::getString($cmd, $arg);
  }
}