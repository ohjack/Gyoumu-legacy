<?php
namespace Oerp\View;

class Wrapper extends \Gulei\Arch\Wrapper
{
  protected $arch;
  protected $fields;
  protected $key;

  function __construct($fields, $arch, $key = '', $col = 4)
  {
    $this->arch = new \Gulei\Arch\Element($arch);
    $this->fields = $fields;
    $this->key = $key;

    if ($col_apply = $this->arch->getArch()->getAttribute('col')) {
      parent::__construct($col_apply);
    }
    else {
      parent::__construct($col);
    }
  }

  static protected function getOpts($arch, $col)
  {
    $opts = array();
    $opts['colspan'] = (int)$arch->getAttribute('colspan');

    if ($opts['colspan']) {
      if ($opts['colspan'] > $col) {
        $opts['colspan'] = $col;
      }
    }
    else {
      switch ($arch->nodeName) {
        case 'form':
        case 'tree':
        case 'page':
        case 'separator':
          $opts['colspan'] = $col;
          break;

        case 'group':
        case 'notebook':
        case 'label':
          $opts['colspan'] = 1;
          break;

        case 'field':
        case 'button':
          $opts['colspan'] = 2;
          break;
      }
    }
    return $opts;
  }

  public function renderForm($ele_src, $build_id)
  {
    $arch = $this->arch->getArch();
    $ele_src = '<div>' . $ele_src . '</div>';
    $EleSrc = new \Gulei\Arch\Element($ele_src);

    $eleState = $EleSrc->xp('//input[@type="hidden"][@state][1]')->item(0);
    if ($eleState) {
      $state = $eleState->getAttribute('state');
    }
    unset($eleState);

    $tag = $arch->nodeName;

    if ($tag != 'tree') {
      foreach ($this->arch->xp('./*', $this->arch->getArch()) as $fld) {
        $ftag = $fld->nodeName;

        if ($ftag == 'newline') {
          $this->addBreak();
          continue;
        }

        if ($fld->getAttribute('invisible')) {
          $name = $fld->getAttribute('name');
          $key = str_replace('_', '-', $name);
          $key = ($this->key) ? $this->key . '-' . $key : $key;

          $Ele = $EleSrc->xp('//input[@id="edit-' . $key . '"]')->item(0);
          $this->addSuffix(
            sprintf(
              '<div oerp_name="%s" oerp_type="%s" invisible="1">%s</div>',
              $name, $this->fields[$name]['type'], $EleSrc->dump($Ele)
            )
          );

          unset($Ele);
          continue;
        }

        $opts = self::getOpts($fld, $this->col);

        if ($fld->getAttribute('nolabel')) {
          $opts['class'] .= ' nolabel';
        }

        switch ($ftag) {
          case 'label':
            $this->addItem($fld->getAttribute('string'), $opts);
            break;

          case 'button':
            $name = $fld->getAttribute('name');

            $states = $fld->getAttribute('states');
            $states = explode(',', $states);
            array_walk($states, 'trim');

            if ($states && !in_array($state, $states)) {
              continue;
            }

            $eleButton =
                $EleSrc->xp('//input[@type="submit"][@name="' . $name . '"][1]')->item(0);

            $this->addItem(sprintf('
            <div class="form-item">
              <div class="field">
                <label><sup>&nbsp;</sup></label>
                %s
              </div>
            </div>
            ', $EleSrc->dump($eleButton)), $opts);
            break;

          case 'form':
          case 'tree':
          case 'field':
          case 'group':
          case 'notebook':
          case 'page':
          case 'separator':
            $innerCol = (int)$fld->getAttribute('col');
            $innerCol = $innerCol ? $innerCol : 4;
            $InnerWrapper = new Wrapper($this->fields, $fld, $this->key, $innerCol);
            $this->addItem($InnerWrapper->renderForm($ele_src, $build_id), $opts);
            break;
        }
      }
    }

    switch ($tag) {
      case 'separator':
        return sprintf('<div class="separator caption">%s</div>',
                       $arch->getAttribute('string'));

      case 'field':
        $key = str_replace('_', '-', $arch->getAttribute('name'));
        $key = ($this->key) ? $this->key . '-' . $key : $key;

        $snippet = $EleSrc->xp('//div[@id="edit-' . $key . '-wrapper"]')->item(0);

        //      handle readonly fields
        if ($arch->getAttribute('readonly')) {
          foreach ($EleSrc->xp('./input | ./textarea', $snippet) as $innerInput) {
            $innerInput->setAttribute('readonly', 'readonly');
          }

          foreach ($EleSrc->xp('./select', $snippet) as $innerInput) {
            $innerInput->setAttribute('disabled', 'disabled');
          }
        }

        //      workaround for <textarea/>
        if (!$snippet) {
          dpr($key);
          dpr($ele_src);
          echo '<pre>';
          throw new \Exception('Field element not found.');
        }

        $snippet = $EleSrc->dump($snippet);
        $snippet = str_replace('%DUMMY%', '', $snippet);

        $name = $arch->getAttribute('name');
        $type = $this->fields[$name]['type'];

        return sprintf('<div class="field" oerp_type="%s" oerp_name="%s" id="oerp-field-%s">%s</div>',
                       $type, $name, $key, $snippet);

      case 'notebook':
        $index = '';
        foreach ($this->arch->xp('./page', $arch) as $page) {
          $cap = $page->getAttribute('string');
          $index .= sprintf('<li><a href="#%s">%s</a></li>',
                            'oerp-tab-' . md5($build_id . $cap), $cap);
        }

        $output = sprintf('
          <div class="notebook awakable" id="oerp-tabs-%s" data-awake="OerpTabs">
            <ul>%s</ul>
            %s
          </div>
          ', md5(microtime()), $index, $this->render());

        return $output;

      case 'page':
        $cap = $arch->getAttribute('string');
        return sprintf('<div class="page" id="%s">%s</div>',
                       'oerp-tab-' . md5($build_id . $cap), $this->render());

      default:
        return $this->render();
    }
  }
}