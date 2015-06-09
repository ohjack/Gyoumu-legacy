<?php
namespace Oerp\View;

class Prompt
{
  private function getCallee($post)
  {
    $basename = __CLASS__ . "\\Callee\\";

    $callees = array(
      $basename . "exeAction",
      $basename . 'getCached',
      $basename . 'getParentView',
      $basename . 'getFormView',
      $basename . 'getTreeView'
    );

    foreach($callees as $callee){
      $Call = new $callee($post);

      if($Call->isApplicable()){
        return $Call->execute();
      }
    }

    throw new \Exception('Callee not found.');
  }

  public function execute()
  {
    $post = $_POST;
    $callee = $this->getCallee($post);

    if($callee){
      $Call = new $callee($post);
      return $Call->execute();
    }
  }
}