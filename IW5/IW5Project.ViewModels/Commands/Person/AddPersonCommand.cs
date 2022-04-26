using System;
using IW5Project.ViewModels.Framework.Commands;
using IW5Project.ViewModels.Framework.ViewModels;

namespace IW5Project.ViewModels.Commands.Person
{
    class AddPersonCommand : CommandBase<ViewModelCollection<Model.Persons.Person>>
        
    {
        public AddPersonCommand(ViewModelCollection<Model.Persons.Person> viewModelCollection) : base(viewModelCollection)
        {

        }

        public override void Execute(object parameter)
        {
            if (this.ViewModel.NewItem == null)
            {
                this.ViewModel.Status = "Object 'Person' was created.";
                this.ViewModel.NewItem = new Model.Persons.Person();
            }
            else
            {
                if (!string.IsNullOrWhiteSpace(this.ViewModel.Name))
                {
                    this.ViewModel.Service.Add(this.ViewModel.NewItem);
                    this.ViewModel.Items.Add(this.ViewModel.NewItem);
                    this.ViewModel.Service.Save();
                    this.ViewModel.NewItem = null;
                    this.ViewModel.Status = "Object 'Person' was saved to the database.";
                }
            }
        }
    }
}