<?php
namespace Gulei\Arch;

abstract class _Abstract
{
  private $save;
  protected $arch;
  protected $attr;

  abstract protected function loadArch($Dom);

//  function __construct($arch, $param = array()){
  function __construct($arch){
    $this->arch = $arch;
    $this->arch = $this->getArch();
//    $this->initEleId();
  }

  static public function createEle($src){
    $doc = new \DOMDocument();
    $doc->loadXML($src);
    return $doc->documentElement;
  }

  protected function initEleId(){
    $c = 0;
    foreach($this->xp('//*') as $ele){
      $ele->setAttribute('ele_id', $c++);
    }
  }

  public function getTag(){
    return $this->getArch()->nodeName;
  }

  public function setArch($arch){
    $this->arch = $arch;
    $this->getArch();
  }

  private function createDoc()
  {
    $newDom = new \DOMDocument();
    if($this->loadArch($newDom) === false){
      dpr($this->arch);
      echo '<pre>';
      throw new \Exception('GuLib: Failed to load XML in \Gulei\Arch\Element.');
    }
    return $newDom;
  }

  public function getArch(){
    if($this->arch instanceof \DOMDocument){
      return $this->arch = $this->arch->documentElement;
    }

    if($this->arch instanceof \DOMElement){
      return $this->arch;
    }

    if(!is_string($this->arch)){
      var_dump($this->arch);
      echo '<pre>';
      throw new \Exception();
    }

    $newDom = $this->createDoc();
    return $this->arch = $newDom->documentElement;
  }

  public function getDoc(){
    if($doc = $this->getArch()->ownerDocument){
      return $doc;
    }else{
      echo '<pre>';
      throw new Exception('\Gulei\Arch\Composer: DOMDocument missing.');
    }
  }

  public function getNodeValues($path, $context = null, $nameAsKey = true){
    $vals = array();
    $context = is_null($context) ? $this->getArch() : $context;

    foreach($this->xp($path, $context) as $ele){
      if($this->xp('./*[1]', $ele)->length){
        $val = \Gulei\Xml\Helper::convToArray($this->dump($ele), false);
        $val = $val[$ele->nodeName];
      }
      else{
        $val = $ele->nodeValue;
      }

      if($nameAsKey){
        $vals[$ele->getAttribute('name')] = $val;
      }
      else{
        $vals[] = $val;
      }
    }
    return $vals;
  }

  public function attr($key = null){
    if($key instanceof \DOMElement){
      $attrs = array();
      foreach($key->attributes as $attr){
        $attrs[$attr->name] = $attr->value;
      }
      return $attrs;
    }

    if(!$this->attr){
      $attrs = array();
      foreach($this->getArch()->attributes as $attr){
        $attrs[$attr->name] = $attr->value;
      }
      $this->attr = $attrs;
    }

    return $key ? $this->attr[$key] : $this->attr;
  }

  public function dump($ele = false, $format = false){
    if(is_null($ele)){
      return '';
    }
    else if($ele === false){
      $src = $this->getDoc()->saveXML();
    }
    else{
      $src = $this->getDoc()->saveXML($ele);
    }

    if($format){
      $newXml = new DOMDocument();
      $newXml->preserveWhiteSpace = false;
      $newXml->formatOutput = true;
      $newXml->loadXML($src);
      return $newXml->saveXML();
    }
    else{
      return $src;
    }
  }

  public function dumpArray($ele = false){
    $src = $this->dump($ele);
    return \Gulei\Xml\Helper::convToArray($src, false);
  }

  public function save(){
    $this->save = $this->dump($this->getArch());
    return $this;
  }

  public function renew(){
    $this->arch = $this->save;
    $this->getArch();
    return $this;
  }

  public function walk($func, $ele = null){
    if(is_null($ele)){
      $ele = $this->getArch();
    }

    call_user_func($func, $ele);

    foreach($this->xp('./*', $ele) as $chd){
      $this->walk($func, $chd);
    }
  }

  public function xp($query, $context = null, $attr_map = null, $xp = null){
    if(is_null($xp)){
      $xp = new \DOMXPath($this->getDoc());
    }

    if(is_null($attr_map)){
      $rs = is_null($context)
        ? $xp->query($query) : $xp->query($query, $context);

      return $rs;
    }

    $attrs = array();
    $rs = is_null($context)
      ? $xp->query($query) : $xp->query($query, $context);

    if(is_array($attr_map)){
      foreach($rs as $ele){
        $each = array();
        foreach($attr_map as $attr_name){
          $each[$attr_name] = $ele->getAttribute($attr_name);
        }
        $attrs[] = $each;
      }
    }
    else{
      foreach($rs as $ele){
        $attrs[] = $ele->getAttribute($attr_map);
      }
    }

    return $attrs;
  }

  private function func_recExtractAttr($attr, $default, $arch, &$vals, $prefix = ''){
    foreach($this->xp('./field', $arch) as $fld){
      $key = $prefix . $fld->getAttribute('name');

      if($this->xp('./field[1]', $fld)->length){
        $this->func_recExtractAttr($attr, $default, $fld, &$vals, $key . '.');
      }
      else{
        if($fld->hasAttribute($attr)){
          $vals[$key] = $fld->getAttribute($attr);
        }
        else if(!is_null($default)){
          $vals[$key] = $fld->getAttribute($default);
        }
      }
    }
  }

  public function extractAttr($attr, $default = null){
    $vals = array();
    $this->func_recExtractAttr($attr, $default, $this->getArch(), $vals);
    return $vals;
  }
}