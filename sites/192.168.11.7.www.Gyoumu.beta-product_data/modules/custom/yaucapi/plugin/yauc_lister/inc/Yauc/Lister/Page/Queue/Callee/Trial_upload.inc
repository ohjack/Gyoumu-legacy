<?php
namespace Yauc\Lister\Page\Queue\Callee;

class Trial_upload extends \Yauc\Lister\Page\Queue\Callee\Upload
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function isTrial()
  {
    return true;
  }

  protected function getRecs()
  {
    $recs = parent::getRecs();
    return array_slice($recs, 0, 1);
  }
}