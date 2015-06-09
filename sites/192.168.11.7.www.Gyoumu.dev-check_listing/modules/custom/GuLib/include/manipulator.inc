<?php

class ManipulatorGuValue
{
  private function fixSpan($subject)
  {
    $span_count = substr_count($subject, '<span');
    $span_close_count = substr_count($subject, '</span>');

    if($span_close_count - $span_count > 0){
      $toks = explode('</span>', $subject);
      $newToks = array();

      for($c = 0; $c <= $span_count; $c++){
        if($c > 0){
          $newToks[] = '</span>' . $toks[$c];
        }
        else{
          $newToks[] = $toks[$c];
        }
      }
      return implode('', $newToks);
    }
    return $subject;
  }

  public function match($subject)
  {
    preg_match('@<span class="gu_value">([\s\S]*)</span>@', $subject, $m);

    if(isset($m[0]))
      $m[0] = $this->fixSpan($m[0]);

    if(isset($m[1]))
      $m[1] = $this->fixSpan($m[1]);

    return $m;
  }

  public function value($subject)
  {
    $m = $this->match($subject);
    return empty($m[0]) ? $subject : $m[1];
  }

  public function replace($newValue, &$subject)
  {
    $m = $this->match($subject);

    $search = empty($m[0]) ? $subject : $m[0];
    $replacement = sprintf('<span class="gu_value">%s</span>', $newValue);

    $subject = str_replace($search, $replacement, $subject);
    return $subject;
  }

  public function wrap(&$subject)
  {
    if(strlen($subject) < 1){
      $subject = '<span class="gu_value"></span>';
    }
    else{
      $newVal = $this->value($subject);

      if(!GuXmlHelper::isXml($newVal)){
        $newVal = htmlspecialchars($newVal);
      }
      $this->replace($newVal, $subject);
    }
  }
}

class ManipulatorRefVar
{
  private $vars;

  public function addRefVar(array $input)
  {
    foreach($input as $name => &$var)
    {
      unset($this->vars[$name]);
      $this->vars[$name] = &$var;
    }
  }

  public function &refVar($name)
  {
    return $this->vars[$name];
  }

  public function refVars()
  {
    $vars = array();
    foreach(func_get_args() as $name)
    {
      $vars = &$this->vars[$name];
    }
    return $vars;
  }

  public function val($name)
  {
    return $this->vars[$name];
  }

  public function vals()
  {
    $vars = array();
    foreach(func_get_args() as $name){
      $vars[$name] = $this->vars[$name];
    }
    return $vars;
  }
}
