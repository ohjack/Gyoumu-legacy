Drupal.behaviors.GuTableController = function(){
  ShowTooltip = function(e)
  {
  	var $this = $(this);
  	var text = $this.next('span.show-tooltip-text');
  	var posi = $this.position();
  	
  	text.show()
  		.css('top', posi.top + $this.height() + 5)
  		.css('left', posi.left);
  
  	return false;
  }
  
  HideTooltip = function(e)
  {
    var $this = $(this);
  	var text = $this.next('span.show-tooltip-text');
  	text.hide();
  }

  $('table.gu-table span.show-tooltip-text')
    .prev('span')
    .hover(ShowTooltip, HideTooltip);


  if(Drupal.tableDrag){
    var funcDragRow = Drupal.tableDrag.prototype.dragRow;
    
    Drupal.tableDrag.prototype.dragRow = function(event, self){
      funcDragRow.call(this, event, self);

      var selIndent = 'div.indentation';

      $(selIndent).each(function(){
        var Tr  = $(this).parents('tr:eq(0)')
        Tr.addClass('grouped');

        if(Tr.next('tr').find(selIndent).length < 1){
          Tr.addClass('tail');
        }

        var TrPrev = Tr.prev('tr');
        TrPrev.addClass('grouped').removeClass('tail');

        if(TrPrev.find(selIndent).length < 1){
          TrPrev.addClass('head');
        }
      });

      $('tr.grouped').each(function(){
        var Tr = $(this);

        if(Tr.hasClass('head')){
          if(Tr.next('tr').find(selIndent).length < 1){
            Tr.removeClass('grouped').removeClass('head');
          }
        }
        else{
          if(Tr.find(selIndent).length < 1){
            Tr.removeClass('grouped').removeClass('tail');

            var TrPrev = Tr.prev('tr');

            if(TrPrev.hasClass('head')){
              TrPrev.removeClass(('head')).remove('grouped');
            }
            else if(TrPrev.hasClass('grouped')){
              TrPrev.addClass('tail');
            }
          }
        }
      });
    }
  }

  new PromptConfirm();
};

function PromptConfirm(){
  var _this = this;

  _this.selSubmit = 'form#gulib-form-form :submit';
  _this.selPrompt = 'div.prompt-confirm';

  var sel = _this.selSubmit + '.needConfirm';

  $(sel).each(function(){
    var $btn = $(this);
    var $view = _this.getView($btn);

    $btn.after('<div class="prompt-confirm"><span class="count"></span><a href="" class="func confirm">Confirm</a> | <a href="" class="func cancel">Cancel</a></div>');
    $btn.parent().find(_this.selPrompt + ' a.func').click(function(ev){
      var opt = $(this);
      ev.preventDefault();

      if(opt.hasClass('cancel')){
        _this.reset($view);
      }
      else{
        opt.parent().prev(':submit').click();
      }
    });
  });

  $(sel).click(function(){
    return _this.click($(this));
  });
}

PromptConfirm.prototype.getView = function($btn)
{
  return $btn.parents('form#gulib-form-form:eq(0)');
};

PromptConfirm.prototype.click = function($btn)
{
  var _this = this;
  var $view = _this.getView($btn);

  if($btn.hasClass('needConfirm')){
    _this.reset($view);
    var c = _this.countChecked($btn, $view);

    if(c < 1){
      return false;
    }

    if ($btn.hasClass('hideUnselected')) {
      _this.hideUncheck($view);
    }

    $btn
        .removeClass('needConfirm')
        .next(_this.selPrompt)
        .css('left', $btn.position().left)
        .show();

    return false;
  }
  else{
    return true;
  }
};

PromptConfirm.prototype.countChecked = function($btn, $view)
{
  var _this = this;
  var count = $view.find('tbody tr.selected').not('[merged]').length;
  $btn.next(_this.selPrompt).find('span.count').text('Count: ' + count);
  return count;
};

PromptConfirm.prototype.hideUncheck = function($view){
  var _this = this;
  var $trs = $view.find('tbody tr');

  $trs.not('.selected').hide().addClass('needConfirm-hidden');
  $('html').scrollTop($trs.filter('.selected').eq(-5).offset().top);
};

PromptConfirm.prototype.reset = function($view){
  var _this = this;

  $(_this.selSubmit).each(function(){
    var btn = $(this);
    var prompt = btn.next(_this.selPrompt);

    if(prompt.css('display') != 'none'){
      $view.find('tr.needConfirm-hidden').show();
      btn.addClass('needConfirm');
      prompt.hide();
    }
  });
};
