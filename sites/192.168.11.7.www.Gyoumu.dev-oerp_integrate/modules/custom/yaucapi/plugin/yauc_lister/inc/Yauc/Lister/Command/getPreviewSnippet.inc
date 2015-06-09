<?php
namespace Yauc\Lister\Command;

class getPreviewSnippet extends \Gulei\Command\WithReference\_Abstract
{
  public function execute($refVar = null)
  {
    $rec = $this->ref->val('rec');
    return sprintf('<span rid="%s" class="ylister preview">Preview</span>', $rec['id']);
  }
}