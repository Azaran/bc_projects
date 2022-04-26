using System;
using System.Collections.Generic;
using System.Linq;
using IW5Project.Model.Persons;
using IW5Project.Model.Ratings;

namespace IW5Project.Model.Movies
{
    public class Movie : BaseModel
    {
        private string _origName;
        private string _czechName;
        private string _genre;
        private string _posterEnding;
        private bool _isPosterSet;
        private string _country;
        private int _length;
        private string _description;

        public string OrigName
        {
            get
            {
                return _origName;
            }
            set
            {
                if (_origName != value)
                {
                    _origName = value;
                    OnPropertyChanged();
                }
            }
        }

        public string Name
        {
            get
            {
                return _czechName;
            }
            set
            {
                if (_czechName != value)
                {
                    _czechName = value;
                    OnPropertyChanged();
                }
            }
        }
        public string Genre
        {
            get
            {
                return _genre;
            }
            set
            {
                if (_genre != value)
                {
                    _genre = value;
                    OnPropertyChanged();
                }
            }
        }
        public string PosterEnding
        {
            get
            {
                return _posterEnding;
            }
            set
            {
                if (_posterEnding != value)
                {
                    _posterEnding = value;
                    OnPropertyChanged();
                }
            }
        }
        public bool IsPosterSet
        {
            get
            {
                return _isPosterSet;
            }
            set
            {
                if (_isPosterSet != value)
                {
                    _isPosterSet = value;
                    OnPropertyChanged();
                }
            }
        }

        public string Country
        {
            get
            {
                return _country;
            }
            set
            {
                if (_country != value)
                {
                    _country = value;
                    OnPropertyChanged();
                }
            }
        }
        public int Length
        {
            get
            {
                return _length;
            }
            set
            {
                if (_length != value)
                {
                    _length = value;
                    OnPropertyChanged();
                }
            }
        }
        public string Description
        {
            get
            {
                return _description;
            }
            set
            {
                if (_description != value)
                {
                    _description = value;
                    OnPropertyChanged();
                }
            }
        }
        // TODO:    přepočítat při změně hodnocení
        public int AvrRating { get; set; }
        
        public ICollection<Rating> Ratings { get; set; }
        public List<Guid> Actors { get; set; }
        public List<Guid> Directors { get; set; }
        public virtual ICollection<PersonMovieRole> Positions { get; set; }

        public Movie(string origName)
        {
            OrigName = origName;
            //poster je defaultne nezadan
            this.IsPosterSet = false;
        }

        public Movie()
        {
            //poster je dafaultne nezadan
            this.IsPosterSet = false;
        }

        public void AddDirector(Guid director)
        {
            if (Directors.All(d => d == director)) return; // reziser uz je u filmu
            Directors.Add(director);
            OnPropertyChanged("Directors");
        }
        public void AddActor(Guid actor)
        {
            if (Actors.All(a => a == actor)) return; // herec uz je u filmu
            Directors.Add(actor);
            OnPropertyChanged("Actors");
        }

        public void AddRating(Rating r)
        {
            Ratings.Add(r);
            if (AvrRating == 0)
            {
                AvrRating = r.Stars;
                return;
            }
                AvrRating += r.Stars;
                var pom = AvrRating / 2.0;
                AvrRating = (int)Math.Ceiling(pom);
            OnPropertyChanged("Ratings");
        }

        public void SetPoster(string ending)
        {
            this.PosterEnding = ending;
            this.IsPosterSet = true;
            OnPropertyChanged("IsPosterSet");
            OnPropertyChanged("PosterEnding");
        }
        public void DeletePoster()
        {
            this.PosterEnding = "";
            this.IsPosterSet = false;
            OnPropertyChanged("PosterEnding");
        }


        // TODO:    Neumí měnit hodnocení, lze ho jen odebrat. Asi uplně zbytečná metoda.
        // 
        /// <summary>
        /// Remove Rating with specific id. Should re-calculate AvrRating.
        /// </summary>
        /// <param name="id">ID of <class name="Rating">Rating</class> object</param>
        public void RemoveRating(Guid id)
        {
            foreach (var rating in Ratings.Where(rating => rating.Id.Equals(id)))
            {
                AvrRating  *= 2;
                AvrRating -= rating.Stars;
                Ratings.Remove(rating);
            }
            if (Ratings.Count == 0) AvrRating = 0;

            OnPropertyChanged("Ratings");
        }
        /// <summary>
        /// Remove one specific Rating. Should re-calculate AvrRating.
        /// </summary>
        /// <param name="r"></param>
        /// 
        public void RemoveRating(Rating r)
        {
            if (Ratings.Count == 1)
            {
                AvrRating = 0;
               // Ratings.Clear();
            }
            AvrRating *= 2;
            AvrRating -= r.Stars;
            Ratings.Remove(r);
            OnPropertyChanged("Ratings");
        }

    }
}
