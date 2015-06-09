<?php

include_once 'GuXml.inc';
//drupal_add_css(drupal_get_path('module', 'GuLib') . '/GuLib.css');

class GuWrapper
{
  protected  $col;
  protected  $rows = array(array());
  protected  $suffix = '';
  
  function __construct($col){
    $this->col = $col;
  }
  
  private function c(){
    return count($this->rows);
  }
  
  public function getSpanned(){
    $spanned = 0;
    
    foreach($this->rows[$this->c() - 1] as $cell){
      $spanned += (int)$cell['colspan'];
    }
    return $spanned;
  }
  
  public function addSuffix($src){
    $this->suffix .= $src;
  }
  
  public function addBreak(){
    $spanned = $this->getSpanned();
    
    if($spanned > 0 && $spanned < $this->col){
      $this->rows[$this->c() - 1][] = array(
        'data' => '',
        'colspan' => $this->col - $this->getSpanned(),
        'class' => 'gu-break',
      );
    }
    $this->rows[] = array();
  }
  
  public function addItem($val, $opts = null){
    if(is_null($opts)){
      $opts = array('colspan' => 1);
    }
    
    if($opts['colspan'] > $this->col) return false;
    
    $spanned = $this->getSpanned();
    
    if($opts['colspan'] > $this->col - $spanned){
      $this->addBreak();
    }
    
    $this->rows[$this->c() - 1][] = array(
      'data' => sprintf('<div class="inner %s">%s</div>',
                        $opts['class'], $val),
      'colspan' => $opts['colspan'],
    );
    return true;
  }
    
  public function render(){
    $this->addBreak();
    
    foreach($this->rows as &$row){
      foreach($row as &$cell){
        $width = (float)$cell['colspan'] / $this->col * 100;
        $cell['style'] = 'width: ' . $width . '%;';
        
        if($width == '100'){
          $cell['class'] = 'single-col';
        }
      }
    }
    
    $src = theme_table(null, $this->rows, array(
      'class' => 'gu-arch',
    ));
    
    return $src . $this->suffix;
  }
}


class GuArchElement
{
  private $save;
  protected $arch;
  protected $attr;
    
//  function __construct($arch, $param = array()){
  function __construct($arch){
    $this->arch = $arch;
    $this->arch = $this->getArch();
//    $this->initEleId();
  }

  static public function createEle($src){
    $doc = new DOMDocument();
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
  
  public function getArch(){
    if($this->arch instanceof DOMDocument){
      return $this->arch = $this->arch->documentElement;
    }
    
    if($this->arch instanceof DOMElement){
      return $this->arch;
    }
    
    if(!is_string($this->arch)){
      dpr($this->arch);
      echo '<pre>';
      throw new Exception();
    }
        
    $newDom = new DOMDocument();
    if($newDom->loadXML($this->arch) === false){
      dpr($this->arch);
      echo '<pre>';
      throw new Exception('GuLib: Failed to load XML in GuArchElement.');
    }
    return $this->arch = $newDom->documentElement;
  }
  
  public function getDoc(){
    if($doc = $this->getArch()->ownerDocument){
      return $doc;
    }else{
      echo '<pre>';
      throw new Exception('GuArch: DOMDocument missing.');
    }    
  }
  
  public function getNodeValues($path, $context = null, $nameAsKey = true){
    $vals = array();
    $context = is_null($context) ? $this->getArch() : $context;
    
    foreach($this->xp($path, $context) as $ele){
      if($this->xp('./*[1]', $ele)->length){
        $val = GuXmlHelper::xml2array($this->dump($ele), false);
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
    if($key instanceof DOMElement){
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
    return GuXmlHelper::xml2array($src, false);
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
      $xp = new DOMXPath($this->getDoc());
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

class GuArch extends GuArchElement
{
  function __construct($arch){
    parent::__construct($arch);

    if(!$this->attr('col')){
      switch($this->getTag()){
        case 'group':
          if($this->attr('type') == 'toolbar'){
            $this->attr['col'] = 6;
          }
          else{
            $this->attr['col'] = 4;
          }
          break;
          
        default:
          $this->attr['col'] = 4;
      }
    }
  }
  
  private function getOpts(){
    $arch = $this->getArch();
    $tag = $this->getTag();
    $opts = array();

//  get colspan  
    $colspan = $arch->getAttribute('colspan');
    
    if($colspan){
//    TODO fixed over spanned  
      $opts['colspan'] = $colspan;
//      $opts['colspan'] = ($colspan > $this->attr('col'))
//        ? $this->attr('col')
//        : $colspan;
    }
    else{ 
      switch($tag){
        case 'form':
        case 'tree':
        case 'page':
        case 'separator':
        case 'notebook':
          $opts['colspan'] = $this->attr('col');
          break;
          
        case 'group':
          $opts['colspan'] = 1;
          break;
          
        case 'field':
          $opts['colspan'] = 2;
          break;
      }
    }
    return $opts;
  }

  public function renderForm($arch_src){
    $arch = $this->getArch();
    $tag = $this->getTag();
    $WrapperLayout = new GuWrapper($this->attr('col'));

    if($tag != 'tree'){
      foreach($this->xp('./*', $this->getArch()) as $fld){
        if($fld instanceof DOMElement){
          $ftag = $fld->nodeName;
          
          switch($ftag){
            case 'newline':
              $WrapperLayout->addBreak();
              break;
              
            case 'tree':            
            case 'form':
            case 'field':
            case 'group':
            case 'notebook':
            case 'page':
            case 'separator':
              if($fld->getAttribute('type') == 'hidden'){
                continue;              
              }
              
              $InnerArch = new GuArch($fld, $this->attr('col'));              
              $opts = $InnerArch->getOpts($this->attr('col'));

              if($fld->getAttribute('nolabel')){
                $opts['class'] .= ' nolabel';
              }
    
              $WrapperLayout->addItem($InnerArch->renderForm($arch_src), $opts);
              break;
          }
        }
      }
    }

    switch($tag){
      case 'separator':
        return sprintf('<div class="separator caption">%s</div>',
          $this->attr('string'));
                  
      case 'tree':
        $ele = $arch_src->xp(sprintf('/root/div[@gu_name="%s"]', 
          $this->attr('name')))
          ->item(0);

        if(!$ele){
          dpr($this->dump($this->getArch()));
          echo '<hr/>';
          dpr($arch_src->dump());
          die;
        }
          
        $ele = $arch_src->dump($ele);
        
        return sprintf(
          '<div class="field" gu_type="%s">%s</div>',
          $this->attr('type'), $ele);
        
      case 'field':
        if($this->xp('.//field', $this->getArch())->length){
          return $WrapperLayout->render();  
        }
        
        $ele = $arch_src->xp(sprintf('/root/div[@gu_name="%s" and @anchor="%s"]', 
          $this->attr('name'), $this->attr('anchor')));

        if(!$ele->length){
          dpr($this->dump($this->getArch()));
          echo '<hr/>';
          dpr($arch_src->dump());
          echo '<pre>';
          throw new Exception('Snippet not found.');
        }
        
        $ele = $ele->item(0);
          
        if($this->attr('readonly')){
          switch($ele->getAttribute('gu_type')){
            case 'select':
              $ele->setAttribute('disabled', 'dispabled');
              break;
              
            default:
              $ele->setAttribute('readonly', 'readonly');
          }
        }
        
        if($ele->getAttribute('gu_type') == 'textarea'){
          $ta = $arch_src->xp('./div/textarea', $ele)->item(0);
          if(!$ta->nodeValue){
            $ta->nodeValue = '_DUMMY_';
          }
          unset($ta);
        }
        
        $ele = $arch_src->dump($ele);
        $ele = str_replace('_DUMMY_', '', $ele);
        
        return sprintf(
          '<div class="field" gu_type="%s">%s</div>',
          $this->attr('type'), $ele);

      case 'notebook':
        $index = '';
        
        foreach($this->xp('./page', $this->getArch()) as $page){
          $cap = $page->getAttribute('string');
          $index .= sprintf('<li><a href="#%s">%s</a></li>',
            'gu-tab-' . $page->getAttribute('ele_id'), $cap);
        }
        
        $output = sprintf('
          <div class="notebook" id="gu-tabs-%s">
            <ul>%s</ul>
            %s
          </div>
          ', $this->attr('ele_id'), $index, $WrapperLayout->render());

        return $output;
        
      case 'page':
        return sprintf('<div class="page" id="%s">%s</div>',
          'gu-tab-' . $this->attr('ele_id'), $WrapperLayout->render());
        
      case 'group':
        return sprintf('
          <div class="group %s">%s</div>
          ', $this->attr('type'), $WrapperLayout->render());

      default:
        return $WrapperLayout->render();
    }    
  }
}

class GuParamArch extends GuArchElement
{
  protected $param;
  
  function __construct($arch, $param = array()){
    parent::__construct($arch);
    $this->param = $param;
  }
  
  protected function getDefaultParam(){
    return array();  
  }
  
  public function getParam($name = null, $override = null){
    $param = array_merge(
      $this->getDefaultParam(),
      $this->getNodeValues('./param[@name]')
    );
    
    $param = array_merge($param, $this->param);
    
    if(is_array($override)){
      $param = array_merge($param, $override);
    }
    
    if(is_null($name)){
      return $param;  
    }
    else{
      return $param[$name];
    }
  }

  public function xp($query, $context = null, $attr_map = null){
    $xp = new DOMXPath($this->getDoc());
    
//  register Namespaces
    if($this->param['ns']){
      foreach($this->param['ns'] as $prefix => $uri){
        $xp->registerNamespace($prefix, $uri);
      }
    }
    
    return parent::xp($query, $context, $attr_map, $xp);
  }  
}

class GuArchTemplate
{
  public $t;
  
  function __construct($arch, $state, $params = array()){
    $arch = preg_replace(
      '/tal:condition="state:(.*)"/U',
      'tal:condition="exists:state/\1"',
      $arch
    );
    
    $fp = file_save_data($arch, file_directory_temp() . '/GuArchTpl.xml');
    $this->t = new PHPTAL($fp);
    $this->t->setOutputMode(PHPTAL::XML);
    
    $this->init($state, $params);
  }
  
  protected function init($state, $params){
    $t = $this->t;
    
    $param_send = new stdClass();
    foreach($params as $key => $val){
      $param_send->$key = $val;
    }

    $t->param = $param_send;
    
    if(!is_null($state)){
      $state_send = new stdClass();
      $state_send->$state = true;
  
      $t->state = $state_send;
    }
  }
  
  public function getArch(){
    return $this->t->execute();
  }
}

interface InterfaceArchImporter
{
  public function getArch();
  public function submit($form, &$form_state);
}
?>