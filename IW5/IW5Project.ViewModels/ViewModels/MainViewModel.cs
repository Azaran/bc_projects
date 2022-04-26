using System;
using IW5Project.Model;
using IW5Project.ViewModels.Framework.ViewModels;

namespace IW5Project.ViewModels
{
    public class MainViewModel : ViewModelBase<BaseModel>
    {
        public MovieRating MovieRating { get; set; }
        public MoviesViewModel MoviesViewModel { get; set; }
        public PersonsViewModel PersonsViewModel { get; set; }

        public MainViewModel()
        {
            this.MoviesViewModel = new MoviesViewModel();
            this.PersonsViewModel = new PersonsViewModel();
        }

        public override void LoadData()
        {
            this.PersonsViewModel.LoadData();
            this.MoviesViewModel.LoadData();
        }

        public override void OnProgramShutdownStarted(object sender, EventArgs e)
        {
            this.PersonsViewModel.SaveData();
            this.MoviesViewModel.SaveData();

            this.PersonsViewModel.Dispose();
        }
    }
}
