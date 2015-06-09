<?php
namespace Oerp\Link;

class Tree
{
  static public function create($text, $vid, $query = array())
  {
    $query['vid'] = $vid;
    return \Gulei\Link\Page::create($text, 'Oerp\Page\TreeView', $query);
  }
}