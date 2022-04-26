using System;
using System.Collections.Generic;
using System.Linq;
using IW5Project.Model.Movies;
using IW5Project.Model.Persons;


namespace IW5Project.Services
{
    /// <summary>
    /// Trida simulujici 2 tabulky DB a M:N vztah mezi nimi
    /// </summary>
    public class Storage
    {

        public Storage()
        {
            AdditionalInfo = new List<Positions>();
            Persons = new List<Person>();
            Movies = new List<Movie>();
        }

        public enum Position { Actor, Director };

        public struct MoviePerson
        {
            public Guid IdMovie;
            public Guid IdPerson;
        }
        public struct Positions
        {
            public Position Position;
            public List<MoviePerson> MPRelationships; 
        }
        public List<Movie> Movies { get; private set; }
        public List<Person> Persons { get; private set; }
        public List<Positions> AdditionalInfo { get; private set; }

        private void AddPosition(Position position)
        {
            if (AdditionalInfo.Any(n => n.Position == position)) return; // pozice uz je vytvorena
            AdditionalInfo.Add(new Positions
            {
                Position = position,
                MPRelationships = new List<MoviePerson>()
            });
        }
        private void AddRelationship(Position position, Guid idMovie, Guid idPerson)
        {
            var category = AdditionalInfo.Single(j => j.Position == position);
            var relationship = category.MPRelationships.SingleOrDefault(r => r.IdMovie == idMovie && r.IdPerson == idPerson);

            if (!relationship.Equals(null)) return; // osoba uz na filmu v dane roli pracuje

            category.MPRelationships.Add(new MoviePerson{IdMovie = idMovie, IdPerson = idPerson});
        }
        public void AddAdditionalInfo(Position position, Guid idMovie, Guid idPerson)
        {
            AddPosition(position);
            AddRelationship(position,idMovie, idPerson);
        }

        public void RemoveAdditionalInfo(Position position, Guid idMovie, Guid idPerson)
        {
            var category = AdditionalInfo.SingleOrDefault(p => p.Position == position);
            var relationship = category.MPRelationships.SingleOrDefault(r => r.IdMovie == idMovie && r.IdPerson == idPerson);

            if (relationship.Equals(null))
                throw new ArgumentException("Person doesnt have provided Position in this Movie", nameof(position)); // osoba nepracuj v dane roli

            category.MPRelationships.Remove(relationship);
        }

    }
}