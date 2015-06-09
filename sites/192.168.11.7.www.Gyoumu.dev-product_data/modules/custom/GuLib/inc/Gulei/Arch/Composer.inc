<?php
namespace Gulei\Arch;

class Composer extends \Gulei\Arch\Element
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

  protected function getOpts(){
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
    $WrapperLayout = new \Gulei\Arch\Wrapper($this->attr('col'));

    if($tag != 'tree'){
      foreach($this->xp('./*', $this->getArch()) as $fld){
        if($fld instanceof \DOMElement){
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

              $InnerArch = new \Gulei\Arch\Composer($fld, $this->attr('col'));
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